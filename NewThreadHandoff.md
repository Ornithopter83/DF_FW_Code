# DF Main/Rod 통합 펌웨어 새 작업 인수인계

Updated: 2026-08-28

## 작업 위치

`C:\Projects\VS\DF_FW_Code\CodexManage`

## 먼저 읽을 파일

1. `AGENTS.md`
2. `DF_FIRMWARE_INTEGRATED_REFACTORING_PLAN.md`
3. `CurrentWork.md`
4. `PROJECT_COMMANDS.md`
5. `tasks/08_독립모듈_상태_Callback_안전성.md`
6. `docs/refactoring/SOURCE_REFACTORING_DRAFT.md`
7. `docs/refactoring/MAIN_DEPENDENCY_INVENTORY.md`
8. `docs/refactoring/ROD_DEPENDENCY_INVENTORY.md`
9. `docs/refactoring/MODULE_MIGRATION_PLAN.md`
10. `docs/STYLE_POLICY.md`

## 확인된 상태

- 변경 전 기준은 `legacy/Vm1.0.9.0`, `legacy/Vr1.0.1.0`이며 활성 소스는 `firmware/DF_Main`, `firmware/DF_Rod`다.
- 두 펌웨어는 ESP32-S3 및 Arduino-ESP32 Core 2.0.17 산출물 근거가 있다.
- 프로젝트 로컬 Arduino CLI 1.5.2-rc.1, ESP32 Core 2.0.17, ESP32Servo 1.1.2가 설치되어 있다.
- Rod의 `Kalman.h`는 비활성 코드에서만 참조되어 설치 대상에서 제외했다.
- Main/Rod 공통 전체 ESP32-S3 FQBN option은 `PROJECT_COMMANDS.md`에 확정되어 있다.
- Git 저장소와 `origin` 구성 및 최초 기준선 commit/push가 완료되어 있다.
- 작업 01-A에서 Main/Rod 소스 40개, 33,276줄과 기존 산출물 해시를 `docs/baseline/SOURCE_BASELINE_MANIFEST.md`에 확정했다.
- 변경 전 원본은 `legacy/`로 이동되어 활성 소스와 분리되어 있다.
- Rod build는 정규화된 `firmware/DF_Rod`를 직접 사용한다. 이전 staging 도구는 `deprecated/tools/stage-rod.cmd`로 이동했다.
- `DF_Firmware.sln`에는 실제 firmware project인 Main/Rod만 둔다. Rod가 Main에 의존해 Solution Build가 두 project를 순차 실행한다.
- Main/Rod IntelliSense는 `vs/DF_Arduino_ESP32S3.props`를 공유한다. Core/SDK/현재 사용 library 경로 207개, 실제 전처리 정의와 forced `Arduino.h`가 설정되어 있다.
- Windows GCC 8.4의 긴 경로 문제 때문에 `arduino-env.cmd`가 저장소를 `X:`에 자동 매핑한다.
- 2026-08-27 VS2022 `Release|x64` 전체 빌드가 성공했다. Main 882,320 bytes, Rod 763,552 bytes 및 필수 flash 파일 8개를 확인했다.
- 배포 산출물은 `bin/release/x64/Vm1.0.9.0`과 `bin/release/x64/Vr1.0.1.0`에 각각 4개만 생성한다. 버전은 `tools/firmware-versions.cmd`에서 관리하며 소스 `Version.cpp`와 함께 갱신한다.
- 작업 03~06에서 활성 제품 분기 단일화, deprecated/legacy 보존, `libraries/DFProtocol` 계약 적용과 Main/Rod 기능 경계 1차 분리를 수행했다. 양쪽 `.ino`는 setup/loop 위임만 포함한다.
- 작업 03~06 직후에는 사용자 지시로 build/test/smoke를 실행하지 않았으며, 이후 작업 08에서 현재 활성 소스의 compile/link와 Main–Rod 기본 통신을 확인했다.
- 활성 `.inc`는 모두 제거되었고 Main/Rod module은 대응 `.cpp/.h` 독립 번역 단위로 전환되어 VS2022 project에 등록되어 있다.
- Main/Rod Application `.cpp`가 setup/loop 구현을 소유한다. 공용 상태는 `DF_*_State.cpp/.h`, 기능 상태는 각 모듈이 소유하며 `DF_*_Internal.h`는 작은 구현 전용 집계 헤더다.
- ESP-NOW callback은 길이 검사와 고정 버퍼 게시만 수행하며 문자열/PID 처리와 로그는 loop로 이동했다. Main encoder ISR과 task 간 event snapshot도 임계영역으로 보호한다.
- Main/Rod 테스트 보드에는 현재 firmware 네 영역 flash/verify와 단독 부팅이 성공했다. 상호 등록 후 ESP-NOW 버전 왕복과 999ms 진동 명령/후속 OFF 전송도 확인했다. 사용자는 릴 연동 LED 점멸 전환과 약 1초 진동을 확인했다. 실제 BLDC/encoder 부하 시험은 남아 있다.
- 작업 07-A~G에서 Main/Rod 의존성, owner/API와 `.inc` 제거 순서를 문서로 확정했다. 코드, build 및 test는 수행하지 않았다.

## 현재 작업

- 다음 작업서: `tasks/08_독립모듈_상태_Callback_안전성.md`
- 완료 단위: 01-A 저장소와 원본 기준선 목록 확정
- 부분 완료 단위: 01-B/01-C 코드 기준 Main/Rod 하드웨어 구성 문서화; 실제 장비 대조 보류
- 완료 단위: 01-D Arduino 참조와 외부 library/FQBN 확정
- 완료 단위: 01-E 기존 산출물 release 기준값 확정
- 완료 단위: 02-A~H Arduino CLI 환경, Main/Rod/통합 script, VS2022 solution, IntelliSense, 실제 build 및 버전별 배포 산출물 관리
- 구현 완료 및 현재 compile/link 확인: 03-A~F, 04-A~F, 05-A~G, 06-A~G
- 완료 단위: 01-F 기준 clean build gate
- 완료 단위: 07-A~G 소스 리팩토링 초안, Main/Rod 의존성 및 작업 08 실행 순서 확정
- 완료 단위: 08-A~G Main/Rod 독립 `.cpp/.h`, VS2022 project 등록, 통합 build
- 완료 단위: 08-H internal compatibility header 축소, State/module owner 정착과 file-scope 상태 제한
- 완료 단위: 08-I ESP-NOW callback, 고정 버퍼와 ISR 경계 안전화, Main/Rod clean build와 VS2022 Rebuild
- 다음 단위: 08-J protocol host test와 정적 정책 검사
- 장비 gate 부분 결과: Main/Rod 테스트 보드는 ROM bootloader `--no-stub`, 115200 baud로 네 flash 영역 검증과 각 버전 응답을 확인했다. Main–Rod 무선 버전 왕복, 999ms 진동 명령/후속 OFF 전송, 릴 연동 LED 점멸과 약 1초 실제 진동을 확인했다. native USB flasher stub 방식은 Main 테스트 보드에서 실패했다.
- 병행 정리: 주석 처리 코드와 테스트/수치 변경 이력은 owner 작업 뒤 별도 변경으로 제거한다. 별도 대형 문서는 만들지 않고 중요 값은 이름 있는 모듈 상수와 짧은 이유만 유지한다.

## 중요한 경계

- Codex가 compile/link 빌드를 직접 수행한다.
- 플래시와 장비 검증은 사용자가 수동 수행하며 장비 게이트에서 중단하고 필요한 명령·시나리오·결과를 요청한다.
- C++11 이후 문법, 람다, 스마트 포인터, namespace를 사용하지 않는다.
- 구 코드는 삭제하지 않고 저장소 루트 `deprecated/`로 격리하며 활성 빌드에서 제외한다.
- 현재 Main/Rod 통신 규격을 물리적 이동과 동시에 변경하지 않는다.
- 작업 08의 A~I와 L은 완료했다. 다음은 자동 검사 08-J이며 장비 gate는 08-K다.

## 보류된 외부 확인

- BLDC/encoder/기타 실제 입출력 HIL 결과
