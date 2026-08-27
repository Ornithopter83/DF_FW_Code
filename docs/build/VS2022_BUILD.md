# Visual Studio 2022 Build

Updated: 2026-08-27

## Solution

`DF_Firmware.sln`은 Visual Studio 2022 NMake solution이다. MSVC가 firmware를 컴파일하지 않으며 각 NMake project가 프로젝트 로컬 Arduino CLI를 호출한다.

| Project | 용도 | Build command |
| --- | --- | --- |
| `DF_Main` | Main 소스 탐색과 개별 build | `tools/build-main.cmd` |
| `DF_Rod` | Rod 소스 탐색과 개별 build | `tools/build-rod.cmd` |

솔루션에는 실제 firmware를 만드는 Main/Rod project만 둔다. Rod project가 Main project에 의존하므로 `Build Solution`은 Main 다음 Rod 순서로 각각 한 번씩 빌드한다. 명령행 통합 빌드에는 `tools/build-all.cmd`를 사용한다.

## 최초 사용

1. `X:` drive가 다른 용도로 사용 중이지 않은지 확인한다. Build script가 저장소를 `X:`에 자동 매핑해 Windows GCC의 긴 경로 문제를 피한다.
2. `PROJECT_COMMANDS.md`의 Core/library 설치 확인 명령을 실행한다.
3. Visual Studio 2022에서 `DF_Firmware.sln`을 연다.
4. `Release | x64`를 선택한다. Debug/Release는 NMake 표시 구성일 뿐 Arduino FQBN은 동일하다.
5. `Build > Build Solution`을 실행한다.

## 결과물

```text
artifacts/
├─ build/
│  ├─ DF_Main/
│  └─ DF_Rod/
└─ firmware/
   ├─ DF_Main/
   │  ├─ DF_Main.ino.bin
   │  ├─ DF_Main.ino.bootloader.bin
   │  ├─ DF_Main.ino.partitions.bin
   │  └─ boot_app0.bin
   └─ DF_Rod/
      ├─ DF_Rod.ino.bin
      ├─ DF_Rod.ino.bootloader.bin
      ├─ DF_Rod.ino.partitions.bin
      └─ boot_app0.bin
```

`artifacts/`는 Git에서 제외된다. 기존 `Vm1.0.9.0/.../build`와 `Vr1.0.1.0/build`는 덮어쓰지 않는다.

Rod 원본 폴더 `Vr1.0.1.0`과 대표 sketch `DF_Rod.ino`의 이름이 달라 Arduino CLI가 원본 폴더를 직접 sketch로 열 수 없다. `build-rod.cmd`는 매 build마다 현재 Rod `.ino/.cpp/.h`만 `artifacts/sketch/DF_Rod`에 새로 복사하고 그 staging sketch를 컴파일한다. 원본은 변경하지 않으며 stale file이 남지 않도록 staging 폴더를 먼저 제거한다.

## Clean/Rebuild

- Solution Clean은 Main/Rod project의 clean command를 호출해 위 artifact 경로만 제거한다. 명령행 통합 clean에는 `tools/clean-all.cmd`를 사용한다.
- 개별 project Clean은 해당 firmware의 build/output 경로만 제거한다.
- Rebuild는 build script 자체가 Arduino CLI `--clean`을 사용하므로 Build와 같은 clean build를 수행한다.

## IntelliSense 범위

Main/Rod NMake project는 `vs/DF_Arduino_ESP32S3.props`를 공유한다. 이 파일에는 다음 항목을 등록했다.

- ESP32 Arduino Core 2.0.17과 ESP32-S3 variant
- ESP32-S3 SDK의 Core 2.0.17 공식 preprocessor include 전체
- 현재 build에서 사용하는 `FS`, `SPI`, `SPIFFS`, `Update`, `WiFi`, `Wire`
- ESP32Servo 1.1.2
- 실제 Arduino compiler 전처리 정의
- Arduino builder가 자동 삽입하는 `Arduino.h` forced include

2026-08-27 MSBuild 속성 평가에서 Main/Rod 각각 207개 include 경로가 모두 존재하고 `Update.h`, `FreeRTOSConfig.h`, `FreeRTOS.h`, `esp_now.h`, `ESP32Servo.h`가 해석됨을 확인했다. 실제 compiler와 최종 include 결정은 Arduino CLI가 담당한다. IntelliSense 표시와 firmware compile 결과가 충돌하면 Arduino CLI 결과를 기준으로 한다.

이미 solution을 열어 둔 상태에서 `.vcxproj` 또는 `.props`가 바뀌면 Visual Studio의 project reload 안내에서 `Reload All`을 선택한다. 안내가 없으면 solution을 닫았다 다시 연 뒤 IntelliSense 재분석이 끝날 때까지 기다린다.

## 안전 경계

- Solution Build는 compile만 수행하며 upload/flash/COM port 접근을 하지 않는다.
- FQBN은 `tools/arduino-env.cmd` 한 곳에서 Main/Rod 공통으로 관리한다.
- Core 3.3.0 또는 시스템 PATH의 Arduino 도구를 사용하지 않는다.

## 확인된 빌드 결과

2026-08-27에 Visual Studio 2022 MSBuild로 `Release|x64` Solution Build를 실행해 exit code 0을 확인했다.

| Target | Application | Bootloader | Partitions | boot_app0 |
| --- | ---: | ---: | ---: | ---: |
| Main | 882,320 bytes | 14,032 bytes | 3,072 bytes | 8,192 bytes |
| Rod | 763,552 bytes | 14,032 bytes | 3,072 bytes | 8,192 bytes |

빌드는 Main 다음 Rod 순서로 각각 한 번 실행됐다. upload, flash 및 COM port 접근은 수행하지 않았다. 기존 release와의 상세 비교는 `docs/releases/BASELINE_RELEASE_MANIFEST.md`를 따른다.
