# DF Main/Rod 통합 펌웨어 현재 작업 상태

Updated: 2026-08-27

## 기준선

- 작업공간: `C:\Projects\VS\DF_FW_Code\CodexManage`
- Git 브랜치: `main`
- 원격 저장소: `https://github.com/Ornithopter83/DF_FW_Code.git`
- 최초 기준선 commit/push: 완료 (2026-08-27)
- 환경 재구성 및 미확정 설정 명령서: `PROJECT_COMMANDS.md`
- Main 기준 소스: `Vm1.0.9.0/DF_Main`
- Rod 기준 소스: `Vr1.0.1.0`
- 확인된 MCU/Core: ESP32-S3, Arduino-ESP32 Core 2.0.17
- 활성 작업서: `tasks/01_현재제품_기준선_확정.md`

## 확인된 현재 구현

- Main의 `DF_Main.ino`는 22,714줄이며 최상위 함수가 약 359개다.
- Rod의 `DF_Rod.ino`는 3,065줄이며 통신, IMU, 설정 모드, 전원 및 입력 처리가 결합되어 있다.
- 두 기존 `.map` 파일은 Arduino-ESP32 Core 2.0.17 경로를 포함한다.
- 프로젝트 로컬 `toolchain/arduino-cli/arduino-cli.exe`가 추가되었고 로컬 설정으로 실행된다. 확인 버전은 1.5.2-rc.1이다.
- `toolchain/arduino-cli.yaml`은 data/downloads/user를 모두 저장소의 Git 제외 `toolchain/` 아래로 격리한다.
- 프로젝트 로컬 `toolchain/arduino-data`에 ESP32 Core 2.0.17과 요구 compiler/tools가 설치되어 있다. 시스템 Core 3.3.0과 분리되어 있다.
- 프로젝트 로컬 library에서 ESP32Servo 1.1.2가 인식된다.
- Rod의 `Kalman.h` 참조는 실제 사용 코드가 `#if 0`에만 있어 현재 제품에는 불필요한 의존성으로 판정했다. 설치하지 않고 이후 활성 코드에서 제거한다.
- Core 제공 참조 외에 Main은 `ESP32Servo.h`, Rod는 `Kalman.h`를 직접 참조하지만 해당 라이브러리 원본과 버전은 작업공간 및 인접 DF 펌웨어 프로젝트에 없다.
- 여러 파일이 `Arduino.h` 대신 `arduino.h`를 사용한다.
- 빌드 및 장비 검증은 사용자 수동 수행 정책이다.

## 목표 구조

- Main과 Rod는 독립 펌웨어로 유지한다.
- 현재 제품 구성만 활성 코드에 남긴다.
- 전체 원본은 `legacy/`, 제거 코드는 `deprecated/`에 둔다.
- 공통 메시지 계약만 `libraries/DFProtocol/`에서 공유한다.
- 자체 코드는 C++98/03 문법, 고유 접두어, namespace 미사용 원칙을 따른다.

## 진행

잔여 작업 8개 (01, 02, 03, 04, 05, 06, 07, 08)

## 작업 정책

- 한 번에 한 문자 작업만 수행한다.
- 수동 게이트에서는 사용자에게 필요한 명령과 확인 항목을 전달하고 결과를 기다린다.
- `deprecated/` 파일은 활성 sketch 폴더 밖에 두고 빌드에서 제외한다.
- C++11 이후 문법과 제품 Variant 전처리 분기를 추가하지 않는다.
- 프로토콜과 장비 동작을 파일 이동 작업에서 변경하지 않는다.
- Arduino CLI와 Core는 Git 제외 `toolchain/`에 프로젝트 로컬로 구성하고 시스템의 Core 3.3.0과 분리한다.

## 표준 검증

- 자동 실행 없음. 정확한 Arduino CLI 명령은 FQBN menu option과 외부 라이브러리 버전 확정 후 작업 02에서 고정한다.
- 모든 코드 변경의 최소 수동 게이트: Main/Rod 대상 빌드 성공, link 성공, firmware 생성 확인.
- 주요 경계 변경 후 수동 smoke test, 프로토콜 변경 후 Main–Rod 통합 시험을 요청한다.

## 다음 작업

작업 01-A: 저장소와 원본 기준선 목록 확정. 작업 01-D는 CLI, Core 2.0.17, ESP32Servo 확인까지 진행되었으며 정확한 FQBN menu options 확정이 남아 있다.
