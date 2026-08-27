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
- 정확한 ESP32-S3 FQBN menu option은 아직 확정되지 않았다.
- Git 저장소와 `origin` 구성 및 최초 기준선 commit/push가 완료되어 있다.

## 현재 작업

- 작업서: `tasks/01_현재제품_기준선_확정.md`
- 다음 단위: 01-A 저장소와 원본 기준선 목록 확정

## 중요한 경계

- 사용자가 빌드, 플래시, 장비 검증을 수동 수행한다.
- 수동 게이트에서 중단하고 필요한 명령·시나리오·결과를 요청한다.
- C++11 이후 문법, 람다, 스마트 포인터, namespace를 사용하지 않는다.
- 구 코드는 삭제하지 않고 저장소 루트 `deprecated/`로 격리하며 활성 빌드에서 제외한다.
- 현재 Main/Rod 통신 규격을 물리적 이동과 동시에 변경하지 않는다.

## 보류된 외부 확인

- 정확한 ESP32-S3 FQBN menu option
- 현재 Main/Rod 기준 빌드 성공
- 실제 장비 flash와 smoke/HIL 결과
