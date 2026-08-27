@echo off
setlocal

call "%~dp0arduino-env.cmd"
if errorlevel 1 exit /b %errorlevel%

set "DF_SKETCH=%DF_REPO_ROOT%\Vm1.0.9.0\DF_Main"
set "DF_BUILD_PATH=%DF_REPO_ROOT%\artifacts\build\DF_Main"
set "DF_OUTPUT_PATH=%DF_REPO_ROOT%\artifacts\firmware\DF_Main"

if not exist "%DF_SKETCH%\DF_Main.ino" (
    echo [ERROR] Main sketch not found: "%DF_SKETCH%\DF_Main.ino"
    exit /b 4
)

if not exist "%DF_BUILD_PATH%" mkdir "%DF_BUILD_PATH%"
if errorlevel 1 exit /b %errorlevel%
if not exist "%DF_OUTPUT_PATH%" mkdir "%DF_OUTPUT_PATH%"
if errorlevel 1 exit /b %errorlevel%

echo [DF Main] Clean build started.
"%DF_ARDUINO_CLI%" --config-file "%DF_ARDUINO_CONFIG%" compile --clean --fqbn "%DF_ARDUINO_FQBN%" --build-path "%DF_BUILD_PATH%" --output-dir "%DF_OUTPUT_PATH%" "%DF_SKETCH%"
set "DF_BUILD_EXIT=%ERRORLEVEL%"
if not "%DF_BUILD_EXIT%"=="0" (
    echo [ERROR] DF Main build failed with exit code %DF_BUILD_EXIT%.
    exit /b %DF_BUILD_EXIT%
)

copy /y "%DF_BOOT_APP%" "%DF_OUTPUT_PATH%\boot_app0.bin" >nul
if errorlevel 1 (
    echo [ERROR] Unable to stage Main boot_app0.bin.
    exit /b 6
)

for %%F in ("%DF_OUTPUT_PATH%\DF_Main.ino.bin" "%DF_OUTPUT_PATH%\DF_Main.ino.bootloader.bin" "%DF_OUTPUT_PATH%\DF_Main.ino.partitions.bin" "%DF_OUTPUT_PATH%\boot_app0.bin") do (
    if not exist "%%~fF" (
        echo [ERROR] Required Main artifact not found: "%%~fF"
        exit /b 5
    )
)

echo [DF Main] Build completed.
for %%F in ("%DF_OUTPUT_PATH%\DF_Main.ino.bin" "%DF_OUTPUT_PATH%\DF_Main.ino.bootloader.bin" "%DF_OUTPUT_PATH%\DF_Main.ino.partitions.bin" "%DF_OUTPUT_PATH%\boot_app0.bin") do echo [ARTIFACT] %%~nxF %%~zF bytes
echo [OUTPUT] "%DF_OUTPUT_PATH%"
exit /b 0
