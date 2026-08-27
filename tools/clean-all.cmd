@echo off
setlocal

call "%~dp0clean-main.cmd"
if errorlevel 1 exit /b %errorlevel%
call "%~dp0clean-rod.cmd"
if errorlevel 1 exit /b %errorlevel%

echo [DF Firmware] Main and Rod artifacts cleaned.
exit /b 0
