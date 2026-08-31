# DF Main/Rod 통합 펌웨어 현재 작업 상태

Updated: 2026-08-31

## BLDC 처리 완료 (2026-08-31)

- 사용자가 최신 코드가 옳다고 확인하고 분석 중지 및 해결 완료 처리를 요청했다. 해당 건은 해결 완료이며 미해결 이슈와 추가 분석·로그 요청은 제거했다.
- 최신 활성 코드는 10bit PWM 반전식 `1023 - (1023 * duty / 255)`과 초기화·정지 PWM 1023을 적용한다. 이 코드를 그대로 유지했다.
- 완료 근거는 사용자 확인이다. Codex가 새 빌드·테스트·장비 검증·업로드를 수행한 것으로 기록하지 않는다.
- 작업 03-I 잔여 0개. 기존 잔여 번호 작업은 4개(01, 08, 09, 10), 작업 10 잔여 H 1개이며 이 건과 무관한 기존 작업은 유지한다.

## 소스 버전 자동 출력 경로 (2026-08-31, 최신)

- 사용자 수정 소스 버전 Main `Vm1.0.10.0`, Rod `Vr1.0.2.0`을 그대로 보존했다. 출력 폴더의 별도 고정 버전값을 없애고 `tools/firmware-versions.cmd`/`.ps1`이 Version.cpp를 매 build/clean 때 읽도록 02-H를 보완했다.
- 소스 버전만 수정·저장하면 VS2022/CLI 모두 같은 버전 폴더를 사용한다. 버전 읽기 실패 시 이전 폴더로 출력하거나 정리하지 않고 중단한다.
- 통합 Release x64 compile/link 성공: `bin/release/x64/Vm1.0.10.0` (Main app 885,216 bytes), `bin/release/x64/Vr1.0.2.0` (Rod app 766,848 bytes). 각 4파일/바이너리 내 버전 문자열 확인 및 오류 시나리오 포함 8개 검사 통과. 근거는 `artifacts/version-output-20260831/`이다.
- 이전 `Vm1.0.9.0`/`Vr1.0.1.3` 폴더의 app은 앞선 사용자 빌드로 이미 새 버전 내용이 포함되어 있었다. 이번에는 이전 파일을 보존했으므로 폴더명만 보고 과거 기준 image로 사용하지 않는다. 새 빌드 결과는 새 버전 폴더를 사용한다.
- source/library 파일과 이전 배포 파일 hash 보존 확인. 이번에는 업로드하지 않았으며 마지막 장비 확인 버전과 소스/빌드 버전은 별개다. 기존 장비 게이트 및 잔여 번호 4개는 그대로다.
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
- Main 활성 소스: `firmware/DF_Main`
- Rod 활성 소스: `firmware/DF_Rod`
- 변경 전 기준 원본: `legacy/Vm1.0.9.0/DF_Main`, `legacy/Vr1.0.1.0`
- 확인된 MCU/Core: ESP32-S3, Arduino-ESP32 Core 2.0.17
- 확정 FQBN: `PROJECT_COMMANDS.md`의 Main/Rod 공통 전체 FQBN
- 소스 리팩토링 초안: `docs/refactoring/SOURCE_REFACTORING_DRAFT.md`
- Main 의존성/owner 초안: `docs/refactoring/MAIN_DEPENDENCY_INVENTORY.md`
- Rod 의존성/owner 초안: `docs/refactoring/ROD_DEPENDENCY_INVENTORY.md`
- 독립 모듈 전환 순서: `docs/refactoring/MODULE_MIGRATION_PLAN.md`
- 다음 활성 작업서: `tasks/10_Rod_무선_FW_업데이트.md`

## 확인된 현재 구현

- 변경 전 Main의 `DF_Main.ino`는 22,714줄이며 최상위 함수가 약 359개였다.
- 변경 전 Rod의 `DF_Rod.ino`는 3,065줄이며 통신, IMU, 설정 모드, 전원 및 입력 처리가 결합되어 있었다.
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
- Rod의 원본 폴더/대표 `.ino` 이름 불일치는 빌드 환경 정리 단계에서 clean staging으로 확인했으며, 현재 활성 소스는 정규화된 `firmware/DF_Rod`를 직접 사용한다.
- Windows GCC 8.4의 긴 경로 문제는 저장소를 `X:`에 자동 매핑해 해결했다.
- 2026-08-27 재현 build application은 Main 882,320 bytes, Rod 763,552 bytes다. 필수 flash 파일 8개 생성을 확인했으며 upload/flash는 수행하지 않았다.
- 배포 산출물은 `bin/<configuration>/<platform>/<version>/`에 대상별 플래시 파일 4개만 둔다. 현재 Release 경로는 Main `Vm1.0.9.0`, Rod OTA 기준 `Vr1.0.1.0`, Rod 현재 복구 `Vr1.0.1.3`로 분리된다. `Vr1.0.1.2`는 과거 시험 산출물이다.
- 작업 03~06에서 현재 Variant 단일화, deprecated/legacy 보존, DFProtocol 공유 계약, Main/Rod 기능 경계 1차 분리를 적용했다. 두 `.ino`에는 setup/loop 위임만 남았다.
- 작업 03~06 변경 이후에는 사용자의 명시적 지시에 따라 build, host test, protocol test 및 장비 smoke를 수행하지 않았다. 직전 성공 산출물은 구조 변경 전 기준일 뿐 현재 소스를 검증하지 않는다.
- 2026-08-28 활성 `.inc` 16개를 모두 독립 `.cpp/.h`로 전환했다. Main/Rod Application `.cpp`가 각각 setup/loop 구현을 소유하며 VS2022 project에 모든 module source/header가 등록되어 있다.
- 공용 상태 owner는 `DF_Main_State.cpp/.h`, `DF_Rod_State.cpp/.h`이며 기능 상태 선언은 각 모듈 헤더가 소유한다. `DF_*_Internal.h`는 구현 전용 집계 헤더로 축소되었고 공개 모듈 헤더에서는 사용하지 않는다.
- Arduino CLI 통합 clean build와 VS2022 Solution Rebuild가 현재 구조에서 성공했다.
- Main/Rod ESP-NOW callback은 길이를 검사한 뒤 고정 128-byte 단일 슬롯에 복사하고, 문자열 분석과 로그는 loop 문맥에서 수행한다. Main encoder ISR과 task 사이의 다중 필드 snapshot도 임계영역으로 일치시켰다.
- 외부 장치가 없는 Main 테스트 보드에서 4개 flash 영역 쓰기/재검증, 12초 시리얼 안정성 및 `Vm1.0.9.0` 버전 응답을 확인했다.
- Rod 테스트 보드에도 현재 배포 파일 4개를 ROM bootloader `--no-stub` 방식으로 기록·재검증했다. `Vr1.0.1.0` 부팅, Main–Rod 상호 등록, 무선 버전 왕복과 999ms 1회 진동 명령/후속 OFF 전송을 확인했다. 사용자는 릴 연동 LED가 점멸 상태로 바뀌고 진동이 약 1초간 동작한 것을 확인했다. BLDC/encoder 부하는 미수행이다.

## Rod 시험 표식 제거·USB 복구·PowerPoint 인수인계 (2026-08-31)

- 사용자 묶음 요청과 “빠른 업데이트는 USB 연결 업로드” 정정에 따라 작업 10-I/J를 추가 수행했다. Main 경유 OTA를 이번 복구에 사용하지 않았다.
- HANDLE 원시 입력 activity callback, 200ms 시험 LED 상태·처리 및 Application 호출만 제거했다. 기존 encoder 전송, 버튼/부팅/연결 LED 및 OTA 수신기는 유지했다. 변경 전 5개 파일은 `deprecated/rod/handle-led-test-Vr1.0.1.2/`에 보존했다.
- 현재 Main은 `Vm1.0.9.0`, Rod는 `Vr1.0.1.3`이다. `tools/build-all.cmd Release x64` compile/link와 `tools/test-protocol.cmd`가 exit 0으로 성공했다. Main bin 885,216 bytes, Rod application 766,485 bytes/bin 766,848 bytes, 대상별 배포 4파일이다.
- 이번 명시적 요청 범위에서 Rod USB 직접 업로드를 수행했다. 대상 버전과 파티션 일치를 먼저 확인하고 ROM `--no-stub`, 921600 baud, 압축 전송으로 `otadata 0xe000`와 `app0 0x10000`만 기록했다. 두 영역 hash/verify 및 재부팅 후 `Vr1.0.1.3` 응답 확인 완료. bootloader/partition/NVS/SPIFFS와 Main은 기록하지 않았다.
- 파티션 확인·기록·검증은 33.86초, application 전송은 약 21.7초였다. 이후 버전 조회 시간은 제외한다. 근거: `artifacts/rod-restore-20260831/build.log`, `usb-upload.log`, `result.json`.
- 인수인계 PowerPoint: `docs/handoff/DF_Firmware_Source_Handoff_2026-08-31.pptx` (140장). VS2022/폴더 소스 탐색, 초기화·loop·callback/ISR·USB/OTA 흐름, 활성 경로의 함수 정의 555개(Main 420/Rod 127/공유 8)를 파일별 표로 수록했다. 전체 시그니처와 직접 호출 목록은 발표자 노트에 있다. 빈 함수와 소스 정의 기준이라는 한계를 표시했다.
- 140장 렌더·개별 시각 검토와 함수 목록 대조 완료. 복구 후 물리 HANDLE LED 관찰 및 VS2022 재분석 화면은 사용자 확인 대기다. 작업 10-H 실패 복구 검증과 기존 장비 게이트를 완료 처리하지 않았다.
- 잔여 번호 작업 4개(01/08/09/10), 작업 10 잔여 문자 1개(H). I/J의 요청 범위는 완료했다. commit/push는 수행하지 않았다.

## VS2022 표시 오류 보완 (2026-08-31)

- 사용자 화면의 `IRAM_ATTR` E0070, `vPortEnterCritical` E0020 및 후속 E0065에 대해 작업 02-G IntelliSense 설정만 보완했다. 작업 10-H는 재개하지 않았다.
- `vs/DF_IntelliSense.h`를 `Arduino.h`보다 먼저 강제 포함하며 MSVC IntelliSense에서만 GCC `__attribute__` 구문을 생략한다. 활성 펌웨어와 Core 헤더는 변경하지 않았다.
- MSBuild 속성 평가로 Main/Rod Debug/Release x64 네 구성의 강제 포함 순서와 파일 존재를 확인했다. `tools/build-all.cmd Release x64` compile/link 성공(exit 0): Main bin 885,216 bytes, Rod bin 767,072 bytes, 대상별 배포 파일 4개. ELF의 `ISR_onTimerHandler()`는 `.iram0.text`에 유지되며 sketch 의존성에 분석 전용 헤더가 없다. VS 화면 재분석 결과는 사용자 확인 대기다.
- 잔여 번호/문자 작업 수는 기존과 같다. 이번 보완으로 장비 게이트를 완료 처리하지 않는다.

## 중간정리: 남은 코드 작업

- 충돌 가능한 전역 심볼, 매크로 및 상수의 고유 접두어 정리
- 초기화 순서, ODR 및 hardware object 수명 검토
- 주석 처리된 구 코드, 테스트 로그와 단순 수치 변경 이력 제거; 현재 제약을 설명하는 짧은 주석만 유지
- 현재 활성 소스의 Main/Rod compile/link, protocol host test와 장비 smoke 검증

독립 `.cpp/.h`, VS2022 편집/참조 구조, 상태 owner 및 callback/ISR 안전화까지 완료했다. protocol host/정적 검사와 실제 장비 검증은 남아 있어 최종 리팩토링 완료 상태는 아니다.

2026-08-28 작업 07에서 초안과 의존성 표를 완성했다. Main 약 17,011줄/확인 함수 338개, Rod 약 2,509줄/확인 함수 56개를 분류했으며, Foundation/Scheduler 해체, 기능 owner의 file-scope 상태, 최소 public API와 작업 08-A~J 순서를 확정했다.

## 목표 구조

- Main과 Rod는 독립 펌웨어로 유지한다.
- 현재 제품 구성만 활성 코드에 남긴다.
- 전체 원본은 `legacy/`, 제거 코드는 `deprecated/`에 둔다.
- 공통 메시지 계약만 `libraries/DFProtocol/`에서 공유한다.
- 자체 코드는 C++98/03 문법, 고유 접두어, namespace 미사용 원칙을 따른다.

## 진행

잔여 번호 작업 4개 (01, 08, 09, 10)

작업 01 잔여 문자 3개 (B, C, G)

작업 02 잔여 문자 0개 (A~H 완료)

작업 03~06 잔여 구현 문자 0개 (검증 게이트 미수행)

작업 07 잔여 문자 0개 (A~G 완료, 문서 분석만 수행)

작업 08 잔여 문자 2개 (J, K)

작업 10 잔여 문자 1개 (H: 실패 복구 검증 및 최종 마감)

## 작업 정책

- 한 번에 한 문자 작업만 수행한다.
- compile/link 빌드는 Codex가 직접 실행한다. 플래시와 장비 게이트에서는 사용자에게 필요한 명령과 확인 항목을 전달하고 결과를 기다린다.
- `deprecated/` 파일은 활성 sketch 폴더 밖에 두고 빌드에서 제외한다.
- C++11 이후 문법과 제품 Variant 전처리 분기를 추가하지 않는다.
- 프로토콜과 장비 동작을 파일 이동 작업에서 변경하지 않는다.
- 주석 정리는 기능 변경과 분리한다. 중요 값은 owner 모듈의 이름 있는 상수로 만들고 별도 문서는 장비 교정에 필요한 경우만 최소 작성한다.
- Arduino CLI와 Core는 Git 제외 `toolchain/`에 프로젝트 로컬로 구성하고 시스템의 Core 3.3.0과 분리한다.

## 표준 검증

- 확정 FQBN을 사용하는 Arduino CLI 명령은 작업 02의 VS2022 build script에 고정했으며 Codex가 직접 실행한다.
- 모든 코드 변경의 최소 자동 게이트: Main/Rod 대상 빌드 성공, link 성공, firmware 생성 확인. 문서 초안만 변경하는 작업 07에는 적용하지 않는다.
- 주요 경계 변경 후 수동 smoke test, 프로토콜 변경 후 Main–Rod 통합 시험을 요청한다.

## 다음 작업

2026-08-31 `Vr1.0.1.2` 최종 VS2022 Release/x64 Rebuild 경고 0/오류 0, protocol host test와 OTA dry run 성공. Main bin 885,216 bytes, Rod bin 767,072 bytes 및 대상별 배포 파일 4개를 확인했다. 이후 사용자 승인으로 기반 유선 설치와 최종 Rod OTA를 수행했다.

10-G 정상 경로 완료: 사용자 승인 후 Main/Rod OTA 기반 유선 설치·verify, Main 경유 Rod `Vr1.0.1.2` OTA, 재부팅 후 무선/직접 버전 조회가 성공했다. OTA 명령 호출~완료 출력은 실행 로그 기준 약 2분 49초(유선 설치 제외)다. 사용자가 HANDLE 점등·정지 후 소등·연속 회전 중 점등 유지까지 확인했다. 다음은 10-H의 전송 중단·재시도·잘못된 이미지 거부·전원 차단/유선 복구 검증과 최종 배포 경계 마감이다. 이후 08-J/K 자동 정책 검사/장비 부하 시험 및 09 통합 릴리스를 진행한다.
