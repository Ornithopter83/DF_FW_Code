@echo off
setlocal
call "%~dp0arduino-env.cmd"
if errorlevel 1 exit /b %errorlevel%

if exist "%DF_REPO_ROOT%\artifacts\build\DF_Main" rmdir /s /q "%DF_REPO_ROOT%\artifacts\build\DF_Main"
if errorlevel 1 exit /b %errorlevel%
if exist "%DF_REPO_ROOT%\artifacts\firmware\DF_Main" rmdir /s /q "%DF_REPO_ROOT%\artifacts\firmware\DF_Main"
if errorlevel 1 exit /b %errorlevel%

echo [DF Main] Artifacts cleaned.
exit /b 0
