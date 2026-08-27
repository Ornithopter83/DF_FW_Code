# Arduino 참조 사전 점검

Updated: 2026-08-27

## 결론

기존 산출물이 있으므로 과거 빌드는 성공한 것으로 판단할 수 있지만, 현재 PC에서 즉시 재현할 준비는 되어 있지 않다. Arduino CLI, 전체 FQBN option, 외부 라이브러리 두 종류를 확정한 뒤 첫 수동 기준 빌드를 수행해야 한다.

## 확인된 사항

- Main/Rod 빌드 디렉터리는 `esp32.esp32.esp32s3`이다.
- 양쪽 `.map`에 `Arduino15\packages\esp32\hardware\esp32\2.0.17` 경로가 존재한다.
- 기존 upload script는 ESP32-S3, DIO, 80 MHz flash, 4 MB flash, 921600 baud를 사용한다.
- Core 또는 bundled library 참조:
  - `Arduino.h`
  - `WiFi.h`
  - `esp_now.h`, `esp_wifi.h`, `esp_pm.h`, `esp_sleep.h`
  - `SPI.h`, `Wire.h`, `SPIFFS.h`, `Update.h`
  - `FreeRTOSConfig.h`
- 프로젝트 로컬 `toolchain/arduino-cli/arduino-cli.exe`가 추가되었다.
- 로컬 설정을 지정한 실행 결과는 Arduino CLI 1.5.2-rc.1이다.
- `toolchain/arduino-cli.yaml`의 data/downloads/user 경로가 모두 프로젝트 로컬 `toolchain/` 아래를 가리키며 시스템 Arduino15와 분리된다.
- Arduino IDE 1.8.19가 `C:\Program Files (x86)\Arduino`에 설치되어 있다.
- 사용자 Arduino data에는 ESP32 Core 3.3.0만 설치되어 있고 기준선 Core 2.0.17은 없다.
- `C:\Users\SUHO_DEV_PC\Documents\Arduino\libraries`에는 ArduinoJson만 있다.
- 프로젝트 로컬 library에는 ESP32Servo 1.1.2가 있고 Arduino CLI가 이를 user library로 인식한다.

## 미확정 또는 위험 항목

### Arduino CLI와 FQBN

- Arduino CLI 설치 위치와 버전이 미확정이다.
- 기본 FQBN은 `esp32:esp32:esp32s3`로 추정되지만 USB CDC, PSRAM, partition 등 menu option 전체는 산출물 경로만으로 확정할 수 없다.
- Core는 기준선 재현이 끝날 때까지 2.0.17로 고정한다.

### 외부 라이브러리

- Main의 `ServoMotor.h`가 `ESP32Servo.h`를 include한다.
- Rod의 `Battery.h`가 `Kalman.h`를 include한다.
- 두 라이브러리의 소스, `library.properties`, 정확한 버전은 작업공간과 `.map`에서 확인되지 않았다.
- 인접한 `C:\Projects\VS\DF_FW_Code` 프로젝트에서도 `ESP32Servo.h`와 `Kalman.h` 사본을 찾지 못했다.
- 현재 기능 플래그가 비활성이어도 헤더가 무조건 include되므로 컴파일 단계에서 라이브러리가 필요할 수 있다.
- 특히 `Kalman.h`는 같은 이름을 제공하는 라이브러리가 여럿일 수 있으므로 임의 패키지를 설치하지 않는다. 기존 성공 PC의 라이브러리 폴더 또는 Arduino IDE verbose build log로 정확한 원본을 확인한다.
- 현재 프로젝트 로컬 경로에도 `Kalman.h`는 없다.

### 대소문자와 내부 헤더

- 다수 프로젝트 헤더가 `arduino.h` 소문자를 사용한다. Windows에서는 해석되지만 Linux CI나 대소문자 구분 파일시스템에서는 실패할 수 있다.
- 활성 코드로 옮길 때 `Arduino.h`로 정규화하고, 그 변경만 별도 빌드 게이트로 확인한다.
- `FreeRTOSConfig.h` 직접 include는 Core 내부 배치에 결합될 수 있다. 실제 사용 심볼을 조사한 뒤 불필요하면 제거하고, 필요하면 Core 2.0.17 기준으로 명시한다.
- Arduino-ESP32 3.x는 ESP-NOW callback API 등에서 차이가 날 수 있으므로 이번 1차 리팩토링에서 Core 업그레이드를 함께 수행하지 않는다.

## 첫 수동 환경 확인에서 필요한 결과

사용자가 다음 출력 또는 화면 정보를 제공하면 작업 02의 정확한 명령을 고정할 수 있다.

```text
arduino-cli version
arduino-cli core list
arduino-cli lib list
Arduino IDE의 현재 ESP32-S3 Board 선택
Tools 메뉴의 모든 활성 Board option
기존 성공 빌드의 verbose compile log
```

## 권장 프로젝트 로컬 경로

시스템의 ESP32 Core 3.3.0과 충돌하지 않도록 다음 경로를 Git에서 제외하고 사용한다.

```text
CodexManage/
└─ toolchain/
   ├─ arduino-cli/
   │  └─ arduino-cli.exe
   ├─ arduino-data/          # Core 2.0.17과 compiler tools
   ├─ arduino-downloads/     # package download cache
   ├─ arduino-user/
   │  └─ libraries/          # ESP32Servo, Kalman
   └─ arduino-cli.yaml
```

- build script는 시스템 PATH 대신 위 `arduino-cli.exe`를 절대 또는 script 상대 경로로 호출한다.
- `arduino-cli.yaml`의 data/downloads/user 경로도 위 폴더로 한정한다.
- `toolchain/` 전체는 `.gitignore` 대상이며, 설치 버전과 출처만 추적 문서에 기록한다.

## 2026-08-27 로컬 경로 실행 결과

- CLI version: 1.5.2-rc.1, 실행 성공
- ESP32Servo: 1.1.2, user library 인식 성공
- ESP32 Core: 설치 없음
- Kalman: 설치 없음
- package/library index: 프로젝트 로컬 data 경로가 새 상태이며 index 파일 없음
- 네트워크가 제한된 실행 환경에서는 index 다운로드가 차단되었으므로 Core 설치는 별도 승인 또는 사용자 수동 설치가 필요하다.

## 수동 게이트 정책

- Codex는 설치, compile, upload, flash를 자동 수행하지 않는다.
- 참조와 스크립트 준비가 끝나면 Main과 Rod 각각의 명령을 제시하고 사용자 실행 결과를 기다린다.
- 최소 필요 결과는 exit code, 마지막 build summary, firmware 크기, 생성된 파일 목록이다.
