@echo off
setlocal
rem Convenience entry point; all build settings live in example.vcxproj.
set "STX_CONFIG=%~1"
set "STX_PLATFORM=%~2"
if not defined STX_CONFIG set "STX_CONFIG=Release"
if not defined STX_PLATFORM set "STX_PLATFORM=x64"
if not "%STX_CONFIG%"=="Debug" if not "%STX_CONFIG%"=="Release" exit /b 2
if not "%STX_PLATFORM%"=="Win32" if not "%STX_PLATFORM%"=="x64" exit /b 2
set "STX_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%STX_VSWHERE%" (
  echo Visual Studio with the Desktop development with C++ workload is required.
  exit /b 1
)
for /f "usebackq tokens=*" %%V in (`"%STX_VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "STX_VS=%%V"
if not defined STX_VS (
  echo No Visual Studio C compiler found.
  exit /b 1
)
"%STX_VS%\MSBuild\Current\Bin\MSBuild.exe" "%~dp0example.sln" /nologo /v:minimal /p:Configuration=%STX_CONFIG% /p:Platform=%STX_PLATFORM%
exit /b %ERRORLEVEL%
