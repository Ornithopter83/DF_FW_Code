# DF Main/Rod 통합 펌웨어 새 작업 인수인계

Updated: 2026-08-27

## 작업 위치

`C:\Projects\VS\DF_FW_Code\CodexManage`

## 먼저 읽을 파일

1. `AGENTS.md`
2. `DF_FIRMWARE_INTEGRATED_REFACTORING_PLAN.md`
3. `CurrentWork.md`
4. `PROJECT_COMMANDS.md`
5. `tasks/01_현재제품_기준선_확정.md`
6. `docs/STYLE_POLICY.md`
7. `docs/build/ARDUINO_REFERENCE_AUDIT.md`

## 확인된 상태

- Main 기준은 `Vm1.0.9.0`, Rod 기준은 `Vr1.0.1.0`이다.
- 두 펌웨어는 ESP32-S3 및 Arduino-ESP32 Core 2.0.17 산출물 근거가 있다.
- 프로젝트 로컬 Arduino CLI 1.5.2-rc.1, ESP32 Core 2.0.17, ESP32Servo 1.1.2가 설치되어 있다.
- Rod의 `Kalman.h`는 비활성 코드에서만 참조되어 설치 대상에서 제외했다.
- Main/Rod 공통 전체 ESP32-S3 FQBN option은 `PROJECT_COMMANDS.md`에 확정되어 있다.
- Git 저장소와 `origin` 구성 및 최초 기준선 commit/push가 완료되어 있다.
- 작업 01-A에서 Main/Rod 소스 40개, 33,276줄과 기존 산출물 해시를 `docs/baseline/SOURCE_BASELINE_MANIFEST.md`에 확정했다.
- 원본은 작업 02까지 현재 경로를 유지하고 작업 03에서 `git mv`로 `legacy/`에 이동한다.
- Rod build는 원본 폴더명 불일치 때문에 `tools/stage-rod.cmd`로 `artifacts/sketch/DF_Rod`를 생성한 뒤 수행한다. 원본/staging 15개 hash 일치와 CLI sketch 인식은 확인됐다.
- `DF_Firmware.sln`에는 실제 firmware project인 Main/Rod만 둔다. Rod가 Main에 의존해 Solution Build가 두 project를 순차 실행한다.
- Main/Rod IntelliSense는 `vs/DF_Arduino_ESP32S3.props`를 공유한다. Core/SDK/현재 사용 library 경로 207개, 실제 전처리 정의와 forced `Arduino.h`가 설정되어 있다.
- Windows GCC 8.4의 긴 경로 문제 때문에 `arduino-env.cmd`가 저장소를 `X:`에 자동 매핑한다.
- 2026-08-27 VS2022 `Release|x64` 전체 빌드가 성공했다. Main 882,320 bytes, Rod 763,552 bytes 및 필수 flash 파일 8개를 확인했다.
- 배포 산출물은 `bin/release/x64/Vm1.0.9.0`과 `bin/release/x64/Vr1.0.1.0`에 각각 4개만 생성한다. 버전은 `tools/firmware-versions.cmd`에서 관리하며 소스 `Version.h`와 함께 갱신한다.

## 현재 작업

- 작업서: `tasks/01_현재제품_기준선_확정.md`
- 완료 단위: 01-A 저장소와 원본 기준선 목록 확정
- 부분 완료 단위: 01-B/01-C 코드 기준 Main/Rod 하드웨어 구성 문서화; 실제 장비 대조 보류
- 완료 단위: 01-D Arduino 참조와 외부 library/FQBN 확정
- 완료 단위: 01-E 기존 산출물 release 기준값 확정
- 완료 단위: 02-A~H Arduino CLI 환경, Main/Rod/통합 script, VS2022 solution, IntelliSense, 실제 build 및 버전별 배포 산출물 관리
- 완료 단위: 01-F 기준 clean build gate
- 다음 단위: 01-B/C/G 실제 Main/Rod 하드웨어 대조 및 사용자 장비 gate

## 중요한 경계

- Codex가 compile/link 빌드를 직접 수행한다.
- 플래시와 장비 검증은 사용자가 수동 수행하며 장비 게이트에서 중단하고 필요한 명령·시나리오·결과를 요청한다.
- C++11 이후 문법, 람다, 스마트 포인터, namespace를 사용하지 않는다.
- 구 코드는 삭제하지 않고 저장소 루트 `deprecated/`로 격리하며 활성 빌드에서 제외한다.
- 현재 Main/Rod 통신 규격을 물리적 이동과 동시에 변경하지 않는다.

## 보류된 외부 확인

- 실제 장비 flash와 smoke/HIL 결과
