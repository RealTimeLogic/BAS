param(
    [string]$Configuration = "Release",
    [string]$Platform = "x86",
    [string]$Make = "make",
    [string[]]$MakeArgs = @("mako.exe")
)

$ErrorActionPreference = "Stop"

function Find-VsDevCmd {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) {
        throw "Could not find vswhere.exe. Install Visual Studio or Visual Studio Build Tools."
    }

    $installPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $installPath) {
        throw "Could not find Visual C++ Build Tools. Install the C++ workload for Visual Studio Build Tools."
    }

    $vsDevCmd = Join-Path $installPath "Common7\Tools\VsDevCmd.bat"
    if (-not (Test-Path $vsDevCmd)) {
        throw "Could not find VsDevCmd.bat: $vsDevCmd"
    }

    return $vsDevCmd
}

function Import-CmdEnvironment {
    param(
        [Parameter(Mandatory=$true)]
        [string]$Command
    )

    $environment = cmd.exe /c "$Command && set"
    if ($LASTEXITCODE -ne 0) {
        throw "Visual Studio environment setup failed."
    }

    foreach ($line in $environment) {
        if ($line -match "^(.*?)=(.*)$") {
            [Environment]::SetEnvironmentVariable($matches[1], $matches[2], "Process")
        }
    }
}

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

$makeCommand = Get-Command $Make -ErrorAction SilentlyContinue
if (-not $makeCommand) {
    throw "Could not find GNU Make. Put make.exe on PATH or pass -Make C:\path\to\make.exe."
}

$vsDevCmd = Find-VsDevCmd
$arch = if ($Platform -match "^(x64|amd64)$") { "amd64" } else { "x86" }
Import-CmdEnvironment "`"$vsDevCmd`" -no_logo -arch=$arch"

Write-Host "Using compiler: $((Get-Command cl.exe).Source)"
Write-Host "Using make: $($makeCommand.Source)"
Write-Host "Building Mako with mako.mk ($Configuration, $arch)"

$debugFlag = if ($Configuration -ieq "Debug") { "1" } else { "0" }
& $makeCommand.Source -f mako.mk "DEBUG=$debugFlag" @MakeArgs
if ($LASTEXITCODE -ne 0) {
    throw "Mako make build failed."
}
