# Build Revalidation (2026-10-12)

User approved rebuilding the products affected by the latest committed changes. Each build ran once; no retry was performed.

| Target | Command | Result | Artifact |
| --- | --- | --- | --- |
| Main firmware | `tools/build-main.cmd Release x64` | exit 0 | `bin/release/x64/Vm1.0.10.0/DF_Main.ino.bin` — 909,696 bytes |
| TestModule | `tools/build-testmodule.cmd Release` | exit 0 | `bin/testmodule/Release/DFTestModule_V041.exe` — 83,113,790 bytes |
| BoardWifiConnector | `BoardWifiConnector/build.ps1 -Mode Publish` | exit 0 | `bin/BoardWifiConnector/Release/BoardWifiConnector.exe` — 161,859,788 bytes |

Main emitted the expected bootloader (14,032 bytes), partition table (3,072 bytes), and `boot_app0.bin` (8,192 bytes). The TestModule build restored the project and published Release output. Connector restore reported all projects up to date and self-contained single-file publish completed.

No protocol tests, GUI/runtime tests, network communication tests, flashing, hardware tests, installation, or shutdown action were performed. Rod firmware source was unchanged and was not rebuilt.
