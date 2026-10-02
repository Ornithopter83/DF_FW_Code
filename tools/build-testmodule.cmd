@echo off
setlocal
set "DF_TM_CONFIGURATION=%~1"
if not defined DF_TM_CONFIGURATION set "DF_TM_CONFIGURATION=Release"
if /I "%DF_TM_CONFIGURATION%"=="Debug" (
    call "%~dp0build-testmodule-debug.cmd"
    goto debug_result
)
if /I not "%DF_TM_CONFIGURATION%"=="Release" exit /b 2
powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File "%~dp0build-testmodule.ps1" -Mode Publish
exit /b %errorlevel%

:debug_result
exit /b %errorlevel%
