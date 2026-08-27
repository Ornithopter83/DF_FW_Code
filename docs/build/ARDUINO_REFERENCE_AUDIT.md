# Arduino 참조 점검

Updated: 2026-08-27

## 결론

Main/Rod 재현 빌드에 필요한 프로젝트 로컬 Arduino CLI, ESP32 Core, 외부 library와 전체 FQBN option을 확정했다. 2026-08-27 Codex가 VS2022 clean build와 산출물 비교를 완료했다.

## 확정 환경

```text
Arduino CLI : 1.5.2-rc.1
ESP32 Core  : esp32:esp32@2.0.17
Main library: ESP32Servo 1.1.2
Rod library : 추가 설치 없음
Board       : ESP32S3 Dev Module
```

전체 FQBN:

```text
esp32:esp32:esp32s3:UploadSpeed=921600,USBMode=hwcdc,CDCOnBoot=cdc,MSCOnBoot=default,DFUOnBoot=default,UploadMode=default,CPUFreq=240,FlashMode=dio,FlashSize=4M,PartitionScheme=default,DebugLevel=none,PSRAM=disabled,LoopCore=1,EventsCore=1,EraseFlash=none,JTAGAdapter=default
```

Main과 Rod가 같은 board option을 사용한다.

## 판정 근거

| 설정 | 값 | 근거 |
| --- | --- | --- |
| Board | `esp32:esp32:esp32s3` | 두 기존 build 폴더명과 map의 ESP32-S3/Core 2.0.17 경로 |
| Upload speed | 921600 | Main/Rod `update.bat`, Core 기본 선택 |
| USB mode | Hardware CDC and JTAG | 두 ELF에 `HWCDC` 구현과 `Serial0` symbol 존재 |
| CDC on boot | Enabled | Main은 `Serial.begin()` 없이 USB `Serial`을 사용하고 두 ELF가 HWCDC를 링크 |
| MSC/DFU on boot | Disabled | 기능 사용 근거 없음, Core 2.0.17 기본값 |
| Upload mode | UART0/Hardware CDC | 기존 upload script와 Core 기본값 |
| CPU | 240 MHz | ESP32-S3 Wi-Fi 구성, Core 기본값 |
| Flash mode | DIO 80 MHz | image header `E9 .. 02 2F`, upload script `--flash_mode dio --flash_freq 80m` |
| Flash size | 4 MB | image header와 upload script `--flash_size 4MB` |
| Partition | Default 4 MB SPIFFS | 기존 Main/Rod partition binary를 decode한 결과가 `default.csv`와 동일 |
| Debug level | None | Core 기본값, 별도 build option 근거 없음 |
| PSRAM | Disabled | 두 ELF에 PSRAM 초기화 경로가 링크되지 않았고 Core 기본값 |
| Loop/Events core | Core 1/Core 1 | Core 기본값, 별도 override 근거 없음 |
| Erase/JTAG | Disabled/Disabled | Core 기본값, 기존 upload script에 erase/JTAG 없음 |

기존 partition table:

```text
nvs      0x009000   20K
otadata  0x00E000    8K
app0     0x010000 1280K
app1     0x150000 1280K
spiffs   0x290000 1408K
coredump 0x3F0000   64K
```

## Core 및 library 참조

Core/bundled:

- `Arduino.h`, `WiFi.h`
- `esp_now.h`, `esp_wifi.h`, `esp_pm.h`, `esp_sleep.h`
- `SPI.h`, `Wire.h`, `SPIFFS.h`, `Update.h`
- `FreeRTOSConfig.h`

외부:

- Main: `ESP32Servo.h` → ESP32Servo 1.1.2
- Rod: 없음

Rod의 `Kalman.h` 관련 객체 코드는 `#if 0` 내부에만 있고 `BAT_KAL_FILTER=0`이다. 임의 Kalman library를 설치하지 않으며 작업 03에서 비활성 include/code를 `deprecated/` 정책에 따라 정리한다.

## 로컬 격리

```text
toolchain/
├─ arduino-cli/arduino-cli.exe
├─ arduino-cli.yaml
├─ arduino-data/
├─ arduino-downloads/
└─ arduino-user/libraries/
```

`toolchain/`은 Git에서 제외한다. 시스템 Core 3.3.0과 프로젝트 기준 Core 2.0.17을 혼용하지 않는다.

Windows용 Xtensa GCC 8.4는 Arduino compile의 `-fno-rtti` 조건에서 긴 저장소 경로 아래 `no-rtti/bits/c++config.h`를 열지 못했다. 단순 `#include <bits/c++config.h>` 직접 시험은 기본 multilib 경로를 사용하므로 이 문제를 재현하지 않는다. `arduino-env.cmd`가 저장소를 `X:`에 자동 매핑하고 `arduino-cli.yaml`이 `X:/toolchain/...`을 사용하도록 고정했다.

## 남은 호환 위험

- 여러 자체 header의 `arduino.h` 소문자 include는 Windows에서는 동작하지만 대소문자 구분 환경에서 실패할 수 있다.
- `FreeRTOSConfig.h` 직접 include는 Core 2.0.17 내부 배치에 결합되어 있다.
- Core 3.x의 ESP-NOW callback API 차이가 있으므로 이번 기준선에서 Core를 올리지 않는다.
- 실제 PCB의 PSRAM 장착 여부와 관계없이 기존 firmware build option은 Disabled로 복구됐다. 하드웨어 확인 전 임의로 Enabled로 바꾸지 않는다.

## 검증 기록

- Arduino CLI `board details`가 전체 FQBN option을 모두 선택값으로 인식했다.
- 기존 ELF symbol, image header, partition binary, upload script를 Main/Rod 양쪽에서 교차 확인했다.
- 2026-08-27 VS2022 `Release|x64` Main/Rod clean build가 exit code 0으로 완료됐다.
- upload와 flash는 실행하지 않았다.

## 빌드 게이트 결과

Main 882,320 bytes, Rod 763,552 bytes application과 대상별 필수 flash 파일 4개를 확인했다. 기존 release와의 상세 비교는 `docs/releases/BASELINE_RELEASE_MANIFEST.md`에 기록했다.
