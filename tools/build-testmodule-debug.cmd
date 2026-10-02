@echo off
powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File "%~dp0build-testmodule.ps1" -Mode Build
exit /b %errorlevel%
