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
  data: C:/Projects/VS/DF_FW_Code/CodexManage/toolchain/arduino-data
  downloads: C:/Projects/VS/DF_FW_Code/CodexManage/toolchain/arduino-downloads
  user: C:/Projects/VS/DF_FW_Code/CodexManage/toolchain/arduino-user
```

저장소 위치가 바뀌면 위 세 directory 경로를 새 절대 경로로 변경한다.

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

## 아직 확정해야 하는 Board option

정확한 compile FQBN 문자열을 만들기 전에 기존 성공 환경 또는 실제 장비 설정에서 다음 값을 확인한다.

```text
USB Mode
USB CDC On Boot
USB DFU On Boot
Upload Mode
CPU Frequency
Flash Mode
Flash Size
Partition Scheme
PSRAM
Arduino Runs On Core
Events Run On Core
```

기존 upload script에서 확인된 값:

```text
Upload Speed : 921600
Chip         : ESP32-S3
Flash Mode   : DIO
Flash Freq   : 80 MHz
Flash Size   : 4 MB
```

확인되지 않은 option은 Arduino CLI 기본값으로 임의 확정하지 않는다.

## 빌드와 검증 정책

- 정확한 Main/Rod compile 명령은 Board option 확정 후 작업 02에서 이 문서에 추가한다.
- Codex는 사용자의 별도 요청 없이 compile, upload, flash 또는 장비 검증을 실행하지 않는다.
- 수동 게이트에 도달하면 Codex가 실행할 명령, 예상 산출물과 전달받아야 할 결과를 제시한다.

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

