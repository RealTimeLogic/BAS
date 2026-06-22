<# QNXBuild.ps1
   Download or update the Mako Server source tree and build Mako Server for QNX
   from Windows PowerShell.

   Initialize the QNX SDP environment before running this script, or pass
   -QnxEnvBat with the path to qnxsdp-env.bat.

   Example:
   .\QNXBuild.ps1 -QnxEnvBat "$env:USERPROFILE\qnx800\qnxsdp-env.bat"
#>

param(
    [string]$QnxEnvBat,
    [string]$Target = $env:TARGET,
    [string]$Make = "make",
    [switch]$NoCompile
)

$ErrorActionPreference = "Stop"

$DeveloperMakoZipUrl = "https://makoserver.net/download/packages/mako.zip"
$DeveloperMakoZipInfoUrl = "https://makoserver.net/documentation/developer-package/"

$Repos = @(
    @{ Dir = "BAS";           Url = "https://github.com/RealTimeLogic/BAS.git" },
    @{ Dir = "BAS-Resources"; Url = "https://github.com/RealTimeLogic/BAS-Resources.git" },
    @{ Dir = "LPeg";          Url = "https://github.com/roberto-ieru/LPeg.git" },
    @{ Dir = "lua-protobuf";  Url = "https://github.com/starwing/lua-protobuf.git" },
    @{ Dir = "CBOR";          Url = "https://github.com/spc476/CBOR.git" }
)

function Abort {
    param([Parameter(Mandatory)][string]$Message)
    throw $Message
}

function Test-Cmd {
    param([Parameter(Mandatory)][string]$Name)
    $null -ne (
        Get-Command $Name -All -ErrorAction SilentlyContinue |
            Where-Object { $_.CommandType -ne "Alias" } |
            Select-Object -First 1
    )
}

function Invoke-External {
    param(
        [Parameter(Mandatory)][string]$FilePath,
        [Parameter(Mandatory)][string[]]$Arguments,
        [Parameter(Mandatory)][string]$ErrorMessage
    )

    & $FilePath @Arguments
    if ($LASTEXITCODE -ne 0) {
        Abort "$ErrorMessage Exit code: $LASTEXITCODE"
    }
}

function Import-CmdEnvironment {
    param(
        [Parameter(Mandatory)][string]$Command,
        [string]$WorkingDirectory
    )

    $cmdLine = if ($WorkingDirectory) {
        "cd /d `"$WorkingDirectory`" && $Command && set"
    } else {
        "$Command && set"
    }

    $environment = cmd.exe /c $cmdLine
    if ($LASTEXITCODE -ne 0) {
        Abort "QNX environment setup failed."
    }

    foreach ($line in $environment) {
        if ($line -match "^(.*?)=(.*)$") {
            [Environment]::SetEnvironmentVariable($matches[1], $matches[2], "Process")
        }
    }
}

function New-TempDirectory {
    param([Parameter(Mandatory)][string]$Prefix)

    $path = Join-Path ([IO.Path]::GetTempPath()) ("$Prefix-$PID-$([Guid]::NewGuid().ToString('N'))")
    New-Item -ItemType Directory -Path $path | Out-Null
    return $path
}

function Remove-TempDirectory {
    param([Parameter(Mandatory)][string]$Path)

    if (-not (Test-Path -LiteralPath $Path)) {
        return
    }

    $resolvedPath = [IO.Path]::GetFullPath($Path)
    $tempRoot = [IO.Path]::GetFullPath([IO.Path]::GetTempPath())
    if (-not $resolvedPath.StartsWith($tempRoot, [StringComparison]::OrdinalIgnoreCase)) {
        Abort "Refusing to remove non-temporary path: $resolvedPath"
    }

    Remove-Item -LiteralPath $resolvedPath -Recurse -Force
}

function Sync-GitRepo {
    param(
        [Parameter(Mandatory)][string]$Dir,
        [Parameter(Mandatory)][string]$Url
    )

    if (Test-Path -LiteralPath (Join-Path $Dir ".git")) {
        $status = & git -C $Dir status --porcelain
        if ($LASTEXITCODE -ne 0) {
            Abort "Could not inspect repository state: $Dir"
        }

        if ($status) {
            Write-Host "Skipping update for $Dir; working tree has local changes"
            return
        }

        Write-Host "Updating $Dir"
        Invoke-External -FilePath "git" -Arguments @("-C", $Dir, "pull", "--ff-only") -ErrorMessage "Updating $Dir failed."
        return
    }

    if (Test-Path -LiteralPath $Dir) {
        Abort "'$Dir' already exists but is not a Git repository."
    }

    Write-Host "Cloning $Dir"
    Invoke-External -FilePath "git" -Arguments @("clone", $Url, $Dir) -ErrorMessage "Cloning $Dir failed."
}

function Get-SqliteAmalgamationUrl {
    if ($env:SQLITEURL) {
        return $env:SQLITEURL
    }

    Write-Host "Fetching latest SQLite amalgamation metadata"
    $html = (Invoke-WebRequest -Uri "https://sqlite.org/download.html" -UseBasicParsing).Content
    $match = [regex]::Match($html, "PRODUCT,[0-9.]+,([0-9]{4}/sqlite-amalgamation-[0-9]+\.zip),")
    if (-not $match.Success) {
        Abort "Could not determine latest SQLite amalgamation URL."
    }

    return "https://sqlite.org/$($match.Groups[1].Value)"
}

function Install-SqliteAmalgamation {
    $sqliteUrl = Get-SqliteAmalgamationUrl
    $tempDir = New-TempDirectory -Prefix "sqlite-amalgamation"
    $sqliteZip = Join-Path $tempDir ([IO.Path]::GetFileName($sqliteUrl))

    try {
        Write-Host "Downloading SQLite amalgamation: $sqliteUrl"
        Invoke-WebRequest -Uri $sqliteUrl -OutFile $sqliteZip -UseBasicParsing

        Expand-Archive -Path $sqliteZip -DestinationPath $tempDir -Force
        $root = Get-ChildItem -LiteralPath $tempDir -Directory |
            Where-Object { $_.Name -like "sqlite-amalgamation-*" } |
            Select-Object -First 1

        if (-not $root) {
            Abort "Downloaded SQLite archive did not contain sqlite-amalgamation-*."
        }

        $srcDir = Join-Path (Get-Location) "src"
        if (-not (Test-Path -LiteralPath $srcDir)) {
            New-Item -ItemType Directory -Path $srcDir | Out-Null
        }

        Copy-Item -Path (Join-Path $root.FullName "*") -Destination $srcDir -Force
        Write-Host "Placed SQLite amalgamation in BAS\src"
    }
    finally {
        Remove-TempDirectory -Path $tempDir
    }
}

function Ask-YesNo {
    param(
        [Parameter(Mandatory)][string]$Prompt,
        [bool]$DefaultYes = $false
    )

    $suffix = if ($DefaultYes) { " (Y/n)" } else { " (y/N)" }
    $reply = Read-Host "$Prompt$suffix"
    if ([string]::IsNullOrWhiteSpace($reply)) {
        return $DefaultYes
    }

    return $reply -ieq "y" -or $reply -ieq "yes"
}

function Open-DeveloperMakoZipInfo {
    try {
        Start-Process $DeveloperMakoZipInfoUrl | Out-Null
    }
    catch {
        Write-Host "Open this page for more information: $DeveloperMakoZipInfoUrl"
    }
}

function Install-DeveloperMakoZip {
    param([Parameter(Mandatory)][string]$TargetZip)

    if (-not (Test-Path -LiteralPath $TargetZip)) {
        Abort "Cannot find mako.zip to replace: $TargetZip"
    }

    Write-Host ""
    Write-Host "Mako Developer Edition mako.zip adds Xedge, LSP-Claw, and local MQTT development tools."
    Write-Host "If you install it, this script backs up the current mako.zip before replacing it."

    if (Ask-YesNo -Prompt "Do you want more information in your browser") {
        Open-DeveloperMakoZipInfo
    }

    if (-not (Ask-YesNo -Prompt "Do you want to install Mako Developer Edition mako.zip")) {
        return
    }

    $tempDir = New-TempDirectory -Prefix "mako-developer-zip"
    $tempZip = Join-Path $tempDir "mako.zip"
    $backup = "$TargetZip.bak-$(Get-Date -Format 'yyyyMMdd-HHmmss')"

    try {
        Write-Host "Downloading Mako Developer Edition mako.zip"
        Invoke-WebRequest -Uri $DeveloperMakoZipUrl -OutFile $tempZip -UseBasicParsing

        Write-Host "Backing up $TargetZip to $backup"
        Copy-Item -LiteralPath $TargetZip -Destination $backup -Force

        Write-Host "Installing Mako Developer Edition mako.zip to $TargetZip"
        Copy-Item -LiteralPath $tempZip -Destination $TargetZip -Force
    }
    finally {
        Remove-TempDirectory -Path $tempDir
    }
}

if ($QnxEnvBat) {
    if (-not (Test-Path -LiteralPath $QnxEnvBat)) {
        Abort "Cannot find QNX environment script: $QnxEnvBat"
    }
    Import-CmdEnvironment "`"$QnxEnvBat`"" -WorkingDirectory (Split-Path -Parent $QnxEnvBat)
}

if (-not $env:QNX_TARGET) {
    Abort "You must initialize the QNX environment first, or pass -QnxEnvBat with the path to qnxsdp-env.bat."
}

foreach ($cmd in @("git", "make", "qcc", "sh", "cp", "zip", "chmod")) {
    if (-not (Test-Cmd $cmd)) {
        Abort "Required command not found on PATH: $cmd"
    }
}

$makeCommand = Get-Command $Make -ErrorAction SilentlyContinue
if (-not $makeCommand) {
    Abort "Could not find make command: $Make"
}

if (Test-Path -LiteralPath "src/BAS.c") {
    Abort "Incorrect use! Do not run this script from inside the BAS directory."
}

foreach ($repo in $Repos) {
    Sync-GitRepo -Dir $repo.Dir -Url $repo.Url
}

if (Test-Path -LiteralPath (Join-Path "BAS-Resources" "build")) {
    Get-ChildItem -LiteralPath (Join-Path "BAS-Resources" "build") -Filter "*.sh" | ForEach-Object {
        Invoke-External -FilePath "chmod" -Arguments @("+x", $_.FullName) -ErrorMessage "Could not mark $($_.Name) executable."
    }
}

$hexeFlags = if ($Target) { "-Vgcc_nto$Target" } else { "-Vgcc_ntox86_64" }
Write-Host "Target compile flag: $hexeFlags"

$env:HEXEFLAGS = $hexeFlags
$env:CFLAGS = "$hexeFlags -DBA_QNX -DBA_64BIT -DBA_FILESIZE64 -DUSE_DBGMON=1 -DNDEBUG -DLUA_USE_LINUX -DMAKO -DUSE_EMBEDDED_ZIP=0 -DUSE_IPV6 -DNO_IPV6_MEMBERSHIP -D_POSIX"
$env:CC = "qcc"
$env:XLIB = "-lz -lsocket -lm"

Push-Location "BAS"
try {
    if (-not (Test-Path -LiteralPath "src/sqlite3.c")) {
        Install-SqliteAmalgamation
    }

    if ($NoCompile -or $env:NOCOMPILE) {
        Write-Host "NOCOMPILE is set: skipping build."
        exit 0
    }

    Write-Host "Building QNX Mako Server with mako.mk"
    & $makeCommand.Source -f mako.mk "OS=QNX" "SHELL=sh"
    if ($LASTEXITCODE -ne 0) {
        Abort "Build failed. make returned $LASTEXITCODE."
    }

    Write-Host "Done building 'mako' for QNX."
    Install-DeveloperMakoZip -TargetZip "mako.zip"
}
finally {
    Pop-Location
}
