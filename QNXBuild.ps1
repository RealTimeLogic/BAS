<# QNXBuild.ps1
   Build Mako Server (BAS) for QNX from Windows PowerShell using native cmdlets.
   - If `git` is present, clones repos.
   - If `git` is NOT present, downloads repo ZIPs, expands, renames, deletes ZIPs.
   - Requires a QNX environment (QNX SDP) set up (qcc/make on PATH).

Run this script in PowerShell as follows:

cmd /c ""$env:USERPROFILE\qnx800\qnxsdp-env.bat" && set" |
ForEach-Object {
    if ($_ -match "^(.*?)=(.*)$") {
        Set-Item -Path "Env:$($matches[1])" -Value $matches[2]
    }
}
irm http://localhost/QNXBuild.ps1 | iex

#>

#--------------------------- Helpers ---------------------------#
function Abort($msg) {
    Write-Host ""
    Write-Host "ERROR: $msg" -ForegroundColor Red
    throw "Script aborted"
}

function Test-Cmd($name) {
    $null -ne (Get-Command $name -ErrorAction SilentlyContinue)
}

function Download-File($Url, $OutFile) {
    try {
        Invoke-WebRequest -Uri $Url -OutFile $OutFile -UseBasicParsing
        return $true
    } catch {
        Write-Host "Download failed: $Url" -ForegroundColor Yellow
        return $false
    }
}

function Expand-Zip($ZipPath, $DestPath) {
    if (Test-Path $DestPath) { Remove-Item -Recurse -Force $DestPath }
    Expand-Archive -Path $ZipPath -DestinationPath $DestPath -Force
}

function Copy-DirectoryContent($SrcDir, $DestDir) {
    if (-not (Test-Path $DestDir)) { New-Item -ItemType Directory -Path $DestDir | Out-Null }
    Get-ChildItem -LiteralPath $SrcDir -Force | ForEach-Object {
        $target = Join-Path $DestDir $_.Name
        if (Test-Path $target) {
            if ($_.PSIsContainer) { Remove-Item -Recurse -Force $target } else { Remove-Item -Force $target }
        }
        Move-Item -LiteralPath $_.FullName -Destination $DestDir
    }
}

function Download-GitHubRepo {
    param(
        [Parameter(Mandatory)][string]$Owner,
        [Parameter(Mandatory)][string]$Repo,
        [Parameter(Mandatory)][string]$DestName
    )
    if (Test-Path $DestName) { return }  # already present

    if (Test-Cmd git) {
        Write-Host "Cloning $Owner/$Repo (git)..."
        git clone "https://github.com/$Owner/$Repo.git" $DestName 2>$null
        if ($LASTEXITCODE -ne 0 -or -not (Test-Path $DestName)) {
            Abort "Cloning $Owner/$Repo failed."
        }
        return
    }

    # No git - try ZIP download (prefer main, then master)
    $tmp     = Join-Path $env:TEMP "$Repo.zip"
    $tmpdir  = Join-Path $env:TEMP "$Repo-extract"
    $zips    = @(
        "https://github.com/$Owner/$Repo/archive/refs/heads/main.zip",
        "https://github.com/$Owner/$Repo/archive/refs/heads/master.zip"
    )

    $downloaded = $false
    foreach ($z in $zips) {
        Write-Host "Downloading ZIP: $z"
        if (Download-File -Url $z -OutFile $tmp) { $downloaded = $true; break }
    }
    if (-not $downloaded) { Abort "Could not download $Owner/$Repo ZIP (main/master)." }

    try {
        if (Test-Path $tmpdir) { Remove-Item -Recurse -Force $tmpdir }
        Expand-Zip -ZipPath $tmp -DestPath $tmpdir
        # ZIP extracts to <Repo>-<branch>. Move that extracted single directory to $DestName.
        $sub = Get-ChildItem -LiteralPath $tmpdir | Where-Object { $_.PSIsContainer } | Select-Object -First 1
        if (-not $sub) { Abort "Unexpected ZIP layout for $Owner/$Repo." }
        Move-Item -LiteralPath $sub.FullName -Destination $DestName
    } finally {
        if (Test-Path $tmp)    { Remove-Item -Force $tmp }
        if (Test-Path $tmpdir) { Remove-Item -Recurse -Force $tmpdir }
    }

    Write-Host "Fetched $Owner/$Repo via ZIP."
}

#----------------------- Environment checks --------------------#
if (-not $env:QNX_TARGET) {
    Abort "You must initialize the QNX environment first (run qnxsdp-env.bat) so QNX_TARGET is set."
}

if (-not (Test-Cmd qcc))  { Abort "QNX compiler (qcc) not found in PATH. Did you run qnxsdp-env.bat?" }
if (-not (Test-Cmd make)) { Abort "make not found in PATH. Use the QNX environment that includes 'make'." }


#----------------------- Ensure zip.exe exists --------------------#
if (-not (Get-Command zip -ErrorAction SilentlyContinue)) {
    Write-Host "zip not found: downloading zip.exe..."
    $zipUrl = "https://realtimelogic.com/downloads/tools/zip.exe"
    $dest   = "$env:TEMP\zip.exe"
    $binDir = "$env:TEMP"
    Invoke-WebRequest $zipUrl -OutFile $dest
    Expand-Archive $dest -DestinationPath $binDir -Force
    $env:Path += ";$binDir"
}

#----------------------- Compiler flags ------------------------#
if ($env:TARGET) {
    $env:HEXEFLAGS = "-Vgcc_nto$($env:TARGET)"
} else {
    $env:HEXEFLAGS = "-Vgcc_ntox86_64"
}
Write-Host "Target compile flag: $env:HEXEFLAGS"

$env:CFLAGS = "$($env:HEXEFLAGS) -DBA_QNX -DBA_64BIT -DBA_FILESIZE64 -DUSE_DBGMON=1 -DNDEBUG -DLUA_USE_LINUX -DMAKO -DUSE_EMBEDDED_ZIP=0 -DUSE_IPV6 -DNO_IPV6_MEMBERSHIP -D_POSIX"
$env:CC     = "qcc"
$env:XLIB   = "-lz -lsocket -lm"

# Safety: don't run inside the BAS source directory
if (Test-Path -LiteralPath "src/BAS.c") {
    Abort "Incorrect use! Do not run this script from inside the BAS directory."
}

#------------------------- Repositories ------------------------#
Download-GitHubRepo -Owner "RealTimeLogic" -Repo "BAS"           -DestName "BAS"
Download-GitHubRepo -Owner "RealTimeLogic" -Repo "BAS-Resources" -DestName "BAS-Resources"
# (Linux script chmods the BAS-Resources build .sh files; not needed on Windows)

Download-GitHubRepo -Owner "roberto-ieru" -Repo "LPeg"          -DestName "LPeg"
Download-GitHubRepo -Owner "starwing"     -Repo "lua-protobuf"  -DestName "lua-protobuf"
Download-GitHubRepo -Owner "spc476"       -Repo "CBOR"          -DestName "CBOR"

#---------------------- SQLite amalgamation --------------------#
Push-Location "BAS"
try {
    if (-not (Test-Path -LiteralPath "src/sqlite3.c")) {
        # Allow override via $env:SQLITEURL; otherwise use the same default as the bash script
        $sqliteUrl = if ($env:SQLITEURL) { $env:SQLITEURL } else { "https://www.sqlite.org/2025/sqlite-amalgamation-3490100.zip" }
        $sqliteZip = Join-Path $env:TEMP ([IO.Path]::GetFileName($sqliteUrl))
        $sqliteTmp = Join-Path $env:TEMP "sqlite-extract"

        Write-Host "Downloading: $sqliteUrl"
        if (-not (Download-File -Url $sqliteUrl -OutFile $sqliteZip)) {
            Abort "Failed to download SQLite amalgamation."
        }

        try {
            Expand-Zip -ZipPath $sqliteZip -DestPath $sqliteTmp
            # Move everything from extracted dir into BAS\src
            $root = Get-ChildItem -LiteralPath $sqliteTmp | Where-Object { $_.PSIsContainer } | Select-Object -First 1
            if (-not $root) { Abort "Unexpected SQLite ZIP layout." }
            Copy-DirectoryContent -SrcDir $root.FullName -DestDir (Join-Path (Get-Location) "src")
        } finally {
            if (Test-Path $sqliteZip) { Remove-Item -Force $sqliteZip }
            if (Test-Path $sqliteTmp) { Remove-Item -Recurse -Force $sqliteTmp }
        }
    }

    #------------------------- Build ---------------------------#
    if ($env:NOCOMPILE) {
        Write-Host "NOCOMPILE is set: skipping build."
        exit 0
    }

    # Use QNX 'make' with the provided makefile
    Write-Host "Building (make -f mako.mk) ..."
    & make -f mako.mk
    if ($LASTEXITCODE -ne 0) { Abort "Build failed (make returned $LASTEXITCODE)." }

    Write-Host "Done building 'mako' for QNX." -ForegroundColor Green
}
finally {
    Pop-Location
}
