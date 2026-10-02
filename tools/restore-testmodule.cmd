@echo off
setlocal
pushd "%~dp0.."
if errorlevel 1 exit /b %errorlevel%
if not exist "global.json" goto missing_sdk
dotnet restore "testModule\DFTestModule.csproj" --use-lock-file --force-evaluate -p:RestoreLockedMode=false -p:Platform=x64
set "DF_RESTORE_EXIT=%errorlevel%"
popd
exit /b %DF_RESTORE_EXIT%
:missing_sdk
echo [ERROR] Run tools\pin-dotnet-sdk.ps1 first.
popd
exit /b 2
