@echo off
setlocal
call "%~dp0arduino-env.cmd"
if errorlevel 1 exit /b %errorlevel%

if exist "%DF_REPO_ROOT%\artifacts\build\DF_Rod" rmdir /s /q "%DF_REPO_ROOT%\artifacts\build\DF_Rod"
if errorlevel 1 exit /b %errorlevel%
if exist "%DF_REPO_ROOT%\artifacts\firmware\DF_Rod" rmdir /s /q "%DF_REPO_ROOT%\artifacts\firmware\DF_Rod"
if errorlevel 1 exit /b %errorlevel%
if exist "%DF_REPO_ROOT%\artifacts\sketch\DF_Rod" rmdir /s /q "%DF_REPO_ROOT%\artifacts\sketch\DF_Rod"
if errorlevel 1 exit /b %errorlevel%

echo [DF Rod] Artifacts cleaned.
exit /b 0
