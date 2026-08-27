@echo off
setlocal

call "%~dp0stage-rod.cmd"
if errorlevel 1 exit /b %errorlevel%

call "%~dp0arduino-env.cmd"
if errorlevel 1 exit /b %errorlevel%

set "DF_SKETCH=%DF_REPO_ROOT%\artifacts\sketch\DF_Rod"
set "DF_BUILD_PATH=%DF_REPO_ROOT%\artifacts\build\DF_Rod"
set "DF_OUTPUT_PATH=%DF_REPO_ROOT%\artifacts\firmware\DF_Rod"
if not exist "%DF_BUILD_PATH%" mkdir "%DF_BUILD_PATH%"
if errorlevel 1 exit /b %errorlevel%
if not exist "%DF_OUTPUT_PATH%" mkdir "%DF_OUTPUT_PATH%"
if errorlevel 1 exit /b %errorlevel%

echo [DF Rod] Clean build started from staged current source.
"%DF_ARDUINO_CLI%" --config-file "%DF_ARDUINO_CONFIG%" compile --clean --fqbn "%DF_ARDUINO_FQBN%" --build-path "%DF_BUILD_PATH%" --output-dir "%DF_OUTPUT_PATH%" "%DF_SKETCH%"
set "DF_BUILD_EXIT=%ERRORLEVEL%"
if not "%DF_BUILD_EXIT%"=="0" (
    echo [ERROR] DF Rod build failed with exit code %DF_BUILD_EXIT%.
    exit /b %DF_BUILD_EXIT%
)

copy /y "%DF_BOOT_APP%" "%DF_OUTPUT_PATH%\boot_app0.bin" >nul
if errorlevel 1 (
    echo [ERROR] Unable to stage Rod boot_app0.bin.
    exit /b 6
)

for %%F in ("%DF_OUTPUT_PATH%\DF_Rod.ino.bin" "%DF_OUTPUT_PATH%\DF_Rod.ino.bootloader.bin" "%DF_OUTPUT_PATH%\DF_Rod.ino.partitions.bin" "%DF_OUTPUT_PATH%\boot_app0.bin") do (
    if not exist "%%~fF" (
        echo [ERROR] Required Rod artifact not found: "%%~fF"
        exit /b 5
    )
)

echo [DF Rod] Build completed.
for %%F in ("%DF_OUTPUT_PATH%\DF_Rod.ino.bin" "%DF_OUTPUT_PATH%\DF_Rod.ino.bootloader.bin" "%DF_OUTPUT_PATH%\DF_Rod.ino.partitions.bin" "%DF_OUTPUT_PATH%\boot_app0.bin") do echo [ARTIFACT] %%~nxF %%~zF bytes
echo [OUTPUT] "%DF_OUTPUT_PATH%"
exit /b 0
