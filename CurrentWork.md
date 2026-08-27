# DF Main/Rod 통합 펌웨어 현재 작업 상태

Updated: 2026-08-27

## 기준선

- 작업공간: `C:\Projects\VS\DF_FW_Code\CodexManage`
- Git 브랜치: `main`
- 원격 저장소: `https://github.com/Ornithopter83/DF_FW_Code.git`
- 최초 기준선 commit/push: 완료 (2026-08-27)
- 환경 재구성 및 미확정 설정 명령서: `PROJECT_COMMANDS.md`
- 소스 및 기존 산출물 기준선: `docs/baseline/SOURCE_BASELINE_MANIFEST.md`
- Main 코드 기준 하드웨어 표: `docs/hardware/MAIN_HARDWARE_BASELINE.md`
- Rod 코드 기준 하드웨어 표: `docs/hardware/ROD_HARDWARE_BASELINE.md`
- 기존 release 산출물 기준: `docs/releases/BASELINE_RELEASE_MANIFEST.md`
- Main 기준 소스: `Vm1.0.9.0/DF_Main`
- Rod 기준 소스: `Vr1.0.1.0`
- 확인된 MCU/Core: ESP32-S3, Arduino-ESP32 Core 2.0.17
- 확정 FQBN: `PROJECT_COMMANDS.md`의 Main/Rod 공통 전체 FQBN
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
- Main의 외부 의존성은 프로젝트 로컬 ESP32Servo 1.1.2로 확정했으며, Rod의 `Kalman.h`는 현재 제품 빌드에 불필요하므로 설치 대상에서 제외했다.
- 여러 파일이 `Arduino.h` 대신 `arduino.h`를 사용한다.
- compile/link 빌드는 Codex가 직접 수행하며 플래시와 장비 검증은 사용자가 수동 수행한다.
- `DF_Firmware.sln`에는 Main/Rod NMake project만 있으며 Rod가 Main에 의존한다. `Release|x64` Solution Build가 실제로 성공했다.
- Main/Rod는 `vs/DF_Arduino_ESP32S3.props`의 공통 IntelliSense 설정을 사용한다. 평가된 경로 207개가 모두 존재하며 `Update.h`, `FreeRTOSConfig.h` 등 핵심 헤더 해석을 확인했다.
- Rod는 원본 폴더/대표 `.ino` 이름 불일치를 `artifacts/sketch/DF_Rod` clean staging으로 해결했고 파일 hash 및 Arduino CLI sketch 인식 검증을 통과했다.
- Windows GCC 8.4의 긴 경로 문제는 저장소를 `X:`에 자동 매핑해 해결했다.
- 2026-08-27 재현 build application은 Main 882,320 bytes, Rod 763,552 bytes다. 필수 flash 파일 8개 생성을 확인했으며 upload/flash는 수행하지 않았다.
- 배포 산출물은 `bin/<configuration>/<platform>/<version>/`에 대상별 플래시 파일 4개만 둔다. 현재 Release 경로는 `Vm1.0.9.0`, `Vr1.0.1.0`으로 분리되며 2026-08-27 VS2022 Rebuild와 SHA-256 일치 검증을 통과했다.

## 목표 구조

- Main과 Rod는 독립 펌웨어로 유지한다.
- 현재 제품 구성만 활성 코드에 남긴다.
- 전체 원본은 `legacy/`, 제거 코드는 `deprecated/`에 둔다.
- 공통 메시지 계약만 `libraries/DFProtocol/`에서 공유한다.
- 자체 코드는 C++98/03 문법, 고유 접두어, namespace 미사용 원칙을 따른다.

## 진행

잔여 작업 7개 (01, 03, 04, 05, 06, 07, 08)

작업 01 잔여 문자 3개 (B, C, G)

작업 02 잔여 문자 0개 (A~H 완료)

## 작업 정책

- 한 번에 한 문자 작업만 수행한다.
- compile/link 빌드는 Codex가 직접 실행한다. 플래시와 장비 게이트에서는 사용자에게 필요한 명령과 확인 항목을 전달하고 결과를 기다린다.
- `deprecated/` 파일은 활성 sketch 폴더 밖에 두고 빌드에서 제외한다.
- C++11 이후 문법과 제품 Variant 전처리 분기를 추가하지 않는다.
- 프로토콜과 장비 동작을 파일 이동 작업에서 변경하지 않는다.
- Arduino CLI와 Core는 Git 제외 `toolchain/`에 프로젝트 로컬로 구성하고 시스템의 Core 3.3.0과 분리한다.

## 표준 검증

- 확정 FQBN을 사용하는 Arduino CLI 명령은 작업 02의 VS2022 build script에 고정했으며 Codex가 직접 실행한다.
- 모든 코드 변경의 최소 자동 게이트: Main/Rod 대상 빌드 성공, link 성공, firmware 생성 확인.
- 주요 경계 변경 후 수동 smoke test, 프로토콜 변경 후 Main–Rod 통합 시험을 요청한다.

## 다음 작업

작업 01-B/C/G 장비 게이트: 실제 Main/Rod PCB 리비전과 장착 부품을 문서와 대조한다. 신규 bootloader는 기존과 flash header는 같지만 내부 segment가 다르므로 사용할 bootloader를 확인한 뒤, 사용자가 flash 및 기본 Main–Rod smoke test를 수행해야 한다.
