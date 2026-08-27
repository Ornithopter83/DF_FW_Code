# DF Firmware 프로젝트 명령서

Updated: 2026-08-27

## 기본 경로

```text
Repository : C:\Projects\VS\DF_FW_Code\CodexManage
Arduino CLI: toolchain\arduino-cli\arduino-cli.exe
CLI Config : toolchain\arduino-cli.yaml
Arduino Data: toolchain\arduino-data
Arduino User: toolchain\arduino-user
```

`toolchain/`은 Git에 포함하지 않는다. 새 PC 또는 새 clone에서는 아래 환경 구성을 다시 수행한다.

## Arduino CLI 로컬 설정

`toolchain/arduino-cli.yaml`:

```yaml
board_manager:
  additional_urls:
    - https://espressif.github.io/arduino-esp32/package_esp32_index.json
directories:
  data: X:/toolchain/arduino-data
  downloads: X:/toolchain/arduino-downloads
  user: X:/toolchain/arduino-user
```

ESP32의 Windows GCC 8.4는 현재 저장소의 긴 절대 경로에서 `no-rtti/bits/c++config.h`를 열지 못한다. Build script는 저장소를 `X:`에 자동 매핑하고 위 짧은 경로를 사용한다. `X:`가 다른 용도로 사용 중이면 build 전에 비워 두거나 `arduino-env.cmd`와 이 설정의 drive letter를 함께 변경한다. 필요하면 `subst X: /d`로 매핑을 해제할 수 있다.

## Core 및 라이브러리 준비

PowerShell에서 저장소 루트를 현재 경로로 둔다.

```powershell
$cli = '.\toolchain\arduino-cli\arduino-cli.exe'
$config = '.\toolchain\arduino-cli.yaml'

& $cli --config-file $config core update-index
& $cli --config-file $config core install esp32:esp32@2.0.17
& $cli --config-file $config lib install 'ESP32Servo@1.1.2'
```

설치 확인:

```powershell
& $cli --config-file $config version
& $cli --config-file $config core list
& $cli --config-file $config lib list
& $cli --config-file $config board details --fqbn esp32:esp32:esp32s3
```

확정된 기준:

```text
Arduino CLI : 1.5.2-rc.1 (현재 로컬 설치본)
ESP32 Core  : 2.0.17
ESP32Servo  : 1.1.2
Base FQBN   : esp32:esp32:esp32s3
```

Rod의 `Kalman.h`는 현재 실행 코드에서 사용되지 않는 의존성으로 판정했으므로 설치하지 않는다. 원본 보존 후 활성 코드 정리 과정에서 include와 죽은 코드를 제거한다.

## 확정 Board option

Main/Rod 공통 전체 FQBN:

```text
esp32:esp32:esp32s3:UploadSpeed=921600,USBMode=hwcdc,CDCOnBoot=cdc,MSCOnBoot=default,DFUOnBoot=default,UploadMode=default,CPUFreq=240,FlashMode=dio,FlashSize=4M,PartitionScheme=default,DebugLevel=none,PSRAM=disabled,LoopCore=1,EventsCore=1,EraseFlash=none,JTAGAdapter=default
```

PowerShell 변수:

```powershell
$fqbn = 'esp32:esp32:esp32s3:UploadSpeed=921600,USBMode=hwcdc,CDCOnBoot=cdc,MSCOnBoot=default,DFUOnBoot=default,UploadMode=default,CPUFreq=240,FlashMode=dio,FlashSize=4M,PartitionScheme=default,DebugLevel=none,PSRAM=disabled,LoopCore=1,EventsCore=1,EraseFlash=none,JTAGAdapter=default'
```

판정 근거는 `docs/build/ARDUINO_REFERENCE_AUDIT.md`에 기록했다.

## 빌드와 검증 정책

- Main clean build script:

```powershell
.\tools\build-main.cmd
```

- Rod clean build script:

```powershell
.\tools\build-rod.cmd
```

- Main/Rod 통합 clean build:

```powershell
.\tools\build-all.cmd
```

각 script는 실패 code를 반환하며 필수 flash file의 존재를 검사한다. 결과물은 `artifacts/firmware/DF_Main`, `artifacts/firmware/DF_Rod`에 생성한다.

## Visual Studio 2022

Solution:

```text
DF_Firmware.sln
```

- Solution에는 실제 firmware project인 `DF_Main`, `DF_Rod`만 둔다.
- `Build Solution`: Main 다음 Rod 순서로 두 project를 각각 한 번씩 build한다.
- 명령행 통합 build에는 `tools/build-all.cmd`를 사용한다.
- 자세한 절차와 결과물 위치: `docs/build/VS2022_BUILD.md`
- Arduino/ESP32-S3 IntelliSense 공통 설정: `vs/DF_Arduino_ESP32S3.props`
- solution이 열린 상태에서 project 파일이 변경되면 `Reload All`을 선택하거나 solution을 다시 연다.
- Solution Build는 compile만 수행하고 upload/flash는 수행하지 않는다.
- Codex는 compile/link 빌드를 직접 실행하고 결과를 기록한다.
- upload, flash 및 장비 검증은 사용자가 수행하며 장비 게이트에서 필요한 명령, 예상 결과와 전달받아야 할 항목을 제시한다.

## Git

```powershell
git status --short --branch
git remote -v
```

원격 저장소:

```text
https://github.com/Ornithopter83/DF_FW_Code.git
```

commit, push 및 tag는 사용자의 명시적 요청이 있을 때만 수행한다.
