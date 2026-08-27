@echo off
setlocal

echo [DF Firmware] Main and Rod clean build started.

call "%~dp0build-main.cmd"
set "DF_MAIN_EXIT=%ERRORLEVEL%"
if not "%DF_MAIN_EXIT%"=="0" (
    echo [ERROR] Combined build stopped after Main failure. Exit code %DF_MAIN_EXIT%.
    exit /b %DF_MAIN_EXIT%
)

call "%~dp0build-rod.cmd"
set "DF_ROD_EXIT=%ERRORLEVEL%"
if not "%DF_ROD_EXIT%"=="0" (
    echo [ERROR] Combined build stopped after Rod failure. Exit code %DF_ROD_EXIT%.
    exit /b %DF_ROD_EXIT%
)

call "%~dp0arduino-env.cmd"
if errorlevel 1 exit /b %errorlevel%

set "DF_MAIN_BIN=%DF_REPO_ROOT%\artifacts\firmware\DF_Main\DF_Main.ino.bin"
set "DF_ROD_BIN=%DF_REPO_ROOT%\artifacts\firmware\DF_Rod\DF_Rod.ino.bin"

if not exist "%DF_MAIN_BIN%" (
    echo [ERROR] Main application binary is missing after combined build.
    exit /b 6
)
if not exist "%DF_ROD_BIN%" (
    echo [ERROR] Rod application binary is missing after combined build.
    exit /b 7
)

echo [DF Firmware] Main and Rod build completed.
for %%F in ("%DF_MAIN_BIN%" "%DF_ROD_BIN%") do echo [APPLICATION] %%~nxF %%~zF bytes
echo [OUTPUT] "%DF_REPO_ROOT%\artifacts\firmware"
exit /b 0
