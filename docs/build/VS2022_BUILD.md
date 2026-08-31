# Visual Studio 2022 Build

Updated: 2026-08-31

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
└─ firmware/               # Arduino CLI 중간 출력
   ├─ DF_Main/
   └─ DF_Rod/

bin/
└─ release/
   └─ x64/
      ├─ Vm1.0.9.0/
      │  ├─ DF_Main.ino.bin
      │  ├─ DF_Main.ino.bootloader.bin
      │  ├─ DF_Main.ino.partitions.bin
      │  └─ boot_app0.bin
      └─ Vr1.0.1.0/
         ├─ DF_Rod.ino.bin
         ├─ DF_Rod.ino.bootloader.bin
         ├─ DF_Rod.ino.partitions.bin
         └─ boot_app0.bin
```

`artifacts/`와 `bin/`은 Git에서 제외된다. `bin/<configuration>/<platform>/<version>`은 build마다 해당 version 폴더를 다시 만들고 정확히 4개 플래시 파일만 복사한다. 현재 version은 `tools/firmware-versions.cmd`에서 관리하며 활성 소스 `Version.cpp` 변경과 함께 갱신한다. `legacy/` 기준 원본과 기존 build 산출물은 덮어쓰지 않는다.

Main/Rod build script는 각각 `firmware/DF_Main`, `firmware/DF_Rod`를 직접 sketch로 사용하고 저장소 `libraries/`를 Arduino library 검색 경로로 전달한다. 변경 전 전체 원본은 `legacy/`에 있고 활성 빌드에서 제외된다.

## Clean/Rebuild

- Solution Clean은 Main/Rod project의 clean command를 호출해 해당 중간 산출물과 현재 Configuration/Platform/version의 `bin` 폴더를 제거한다. 명령행 통합 clean에는 `tools/clean-all.cmd`를 사용한다.
- 개별 project Clean은 해당 firmware의 중간 산출물과 배포 폴더만 제거한다.
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

### GCC 속성 관련 표시 오류

`IRAM_ATTR` E0070, FreeRTOS `vPortEnterCritical` E0020와 함수 내부 E0065는 GCC 속성 구문을 VS가 해석하지 못할 때 연쇄적으로 나타날 수 있다. `vs/DF_IntelliSense.h`는 공통 NMake forced include의 첫 파일이며, `__INTELLISENSE__`와 `_MSC_VER`가 정의된 분석에서만 `__attribute__((...))`를 생략한다. 실제 ESP32 compile/link에서는 이 헤더를 포함하지 않고 원래 속성과 IRAM 배치를 유지한다. VS의 분석 결과로 구조체 배치나 target ABI를 검증하지 않는다.

설정 변경 후 `Reload All`을 선택하거나 솔루션을 닫았다 다시 열고 IntelliSense 재분석을 기다린다. 필요하면 솔루션의 `Rescan Solution`을 실행한다. Solution Clean은 펌웨어 산출물만 정리하므로 이 설정을 초기화하지 않는다. 오류 표시 기능이나 자동 완성/F12를 비활성화할 필요는 없다.

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

같은 날 배포 경로 적용 후 `/t:Rebuild`를 다시 실행해 `bin/release/x64/Vm1.0.9.0`과 `bin/release/x64/Vr1.0.1.0`에 각각 정확히 4개 파일이 생성됨을 확인했다. 8개 파일은 각각 `artifacts/firmware`의 대응 파일과 SHA-256이 일치했다.

위 성공 결과는 작업 03~06 구조 변경 전 기준이다. 2026-08-27 구조 변경 이후에는 사용자 지시에 따라 VS/Arduino build, host test와 장비 시험을 실행하지 않았으므로 현재 활성 소스의 결과로 간주하면 안 된다.
