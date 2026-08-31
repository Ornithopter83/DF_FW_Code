# DF Firmware 스레드 이주 명령서

Updated: 2026-08-31

## 인수인계 요청

이 문서를 읽고 기존 작업의 정책과 확인된 상태만 인수인계한다. 사용자의 요청에 따라 후속 작업, 권장 순서 및 신규 작업 지시는 포함하지 않는다. 아래 참조 문서에 실행 계획이 있더라도 자동으로 재개하지 말고, 인수인계한 상태를 짧게 보고한 뒤 사용자의 새 지시를 기다린다.

## 작업 위치와 참조

- 저장소 루트: `C:\Projects\VS\DF_FW_Code\CodexManage`
- 원격 저장소: `https://github.com/Ornithopter83/DF_FW_Code.git`
- 읽는 순서: `AGENTS.md` → `DF_FIRMWARE_INTEGRATED_REFACTORING_PLAN.md` → `CurrentWork.md` → `PROJECT_COMMANDS.md` → `tasks/10_Rod_무선_FW_업데이트.md`
- 초기 commit/push는 완료되어 있다. 현재 OTA/HANDLE 관련 소스·도구·문서에는 수정 및 미추적 파일이 남아 있다. 새 스레드에서는 실제 Git 상태를 읽기 전용으로 확인하고 기존 변경을 보존한다.
- 작업서의 과거 결과와 최신 결과를 구분한다. 현재 버전·장비 결과는 아래 2026-08-31 기록을 기준으로 인수인계한다.

## 유지할 정책

- 최신 Main/Rod 구성만 지원한다. 기준 원본은 `legacy/Vm1.0.9.0`, `legacy/Vr1.0.1.0`이다.
- 활성 코드에서 제거한 구 코드는 루트 `deprecated/`에 별도 파일로 보존한다. 활성 빌드/include에서 제외하며 현대화하지 않는다.
- 자체 작성 코드는 C++98/03 및 C 스타일 호환성을 우선한다. 람다, 스마트 포인터, `auto`, `nullptr`, `constexpr` 등 C++11 이후 문법과 namespace는 사용하지 않는다.
- 전역 심볼은 고유 접두어로 구분한다. 제품 Variant용 조건부 컴파일을 추가하지 않는다. 헤더는 `#pragma once`, 상수는 C++03 호환 선언을 사용한다.
- 문서와 주석은 최소화한다. 단순 수치 변경 이력이나 죽은 코드를 활성 소스에 쌓지 않는다.
- 문자 단위 작업 하나씩 수행하며 명시적인 묶음 요청이 있을 때만 범위를 넓힌다.
- compile/link는 Codex가 수행하고 사실대로 기록한다. 플래시·장비 시험은 기본적으로 사용자 수동 수행이다.
- 2026-08-31 직접 업로드 승인은 완료된 해당 시험에 한정된다. 새 스레드에 상시 업로드 권한이 부여된 것이 아니다.
- 명시적인 요청 없이 commit, push, flash, 배포 및 외부 시스템 변경을 하지 않는다. 비밀정보와 개인 장비 주소를 기록하지 않는다.

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
## 현재 소스와 개발 환경

- 솔루션: `DF_Firmware.sln`. 실제 프로젝트는 `vs/DF_Main.vcxproj`, `vs/DF_Rod.vcxproj` 두 개이며 Main 다음 Rod를 빌드한다.
- 활성 소스: `firmware/DF_Main`, `firmware/DF_Rod`. 공유 통신 코드는 `libraries/DFProtocol`이다.
- 양쪽 `.ino`는 setup/loop 위임만 남겼다. 활성 `.inc`는 독립 `.cpp/.h`로 전환하고 VS2022 프로젝트에 등록했다.
- 공용 상태는 `DF_*_State.cpp/.h`, 기능 상태는 해당 모듈이 소유한다. `DF_*_Internal.h`는 구현 전용 집계 헤더다.
- ESP-NOW callback은 고정 버퍼에 수신 정보만 게시하고 분석·로그·flash 기록은 loop에서 수행한다. Main encoder ISR과 task 간 상태 공유에는 임계영역을 적용했다.
- ESP32-S3, Arduino-ESP32 Core 2.0.17, ESP32Servo 1.1.2, 로컬 Arduino CLI 1.5.2-rc.1을 사용한다. 제품 설정은 4MB default partition이다.
- 로컬 도구는 `toolchain/`에 격리했다. Windows GCC 8.4 긴 경로 문제는 저장소를 `X:`로 매핑하여 해결했다. 상세 옵션은 `PROJECT_COMMANDS.md`를 참조한다.
- 배포 경로는 `bin/<configuration 소문자>/<platform>/<version>/`이며 대상별 application, bootloader, partitions, `boot_app0.bin` 네 파일만 둔다. `bin/`, `artifacts/`, `toolchain/`은 Git 제외다.
- 버전의 단일 기준은 각 `Version.cpp`의 `mainVer`/`rodVer`다. `tools/firmware-versions.cmd`/`.ps1`이 매 build/clean 때 소스를 읽는다. 별도 스크립트 버전값 수정은 필요 없다. 날짜가 고정된 PowerPoint의 동시 수정 안내는 작성 당시 방식이며 현재는 이 동적 방식을 따른다.

## 최신 상태: USB 복구 및 소스 인수인계 (2026-08-31)

- 사용자의 “ROD 시험 점멸 제거·빠른 업데이트·PowerPoint” 요청과 “빠른 업데이트는 USB 직접 업로드” 정정을 처리했다. Main 경유 OTA를 이번 복구에는 사용하지 않았다.
- 현재 Main `Vm1.0.9.0`, Rod `Vr1.0.1.3`. HANDLE activity callback/200ms 시험 LED만 제거하고 원래 encoder 송신·버튼/부팅/연결 LED·OTA 기능은 유지했다. 제거 전 소스는 `deprecated/rod/handle-led-test-Vr1.0.1.2/`에 보존했다.
- `tools/build-all.cmd Release x64` 및 `tools/test-protocol.cmd` exit 0. Main bin 885,216 bytes, Rod application 766,485 bytes/bin 766,848 bytes. 현재 Rod 배포 폴더는 `bin/release/x64/Vr1.0.1.3`이다.
- 사용자 요청 범위에서 Rod USB 직접 업로드 완료. 대상 버전·파티션 일치 후 ROM no-stub 921600 baud 압축 전송으로 otadata/app0만 기록했다. hash/verify 및 재부팅 `Vr1.0.1.3` 응답을 확인했다. 확인·기록·검증 33.86초, application 기록 약 21.7초이며 후속 버전 조회는 제외한다. Main/bootloader/partition/NVS/SPIFFS는 기록하지 않았다.
- 업로드 근거: `artifacts/rod-restore-20260831/usb-upload.log`, `result.json`, `build.log`. 물리 HANDLE LED 복구 동작은 사용자 확인 대기다.
- 소스 인수인계 문서는 `docs/handoff/DF_Firmware_Source_Handoff_2026-08-31.pptx`다. 총 140장, 소스 탐색/흐름 및 함수 정의 555개의 표와 발표자 노트. 전체 렌더·개별 검토와 목록 대조를 수행했다.
- VS2022 표시 오류는 `vs/DF_IntelliSense.h`를 Arduino.h보다 먼저 강제 포함해 MSVC IntelliSense에서만 GCC attribute를 생략하도록 보완했다. 실제 build와 ISR IRAM 배치는 확인했으며 VS 재분석 화면은 사용자 확인 대기다.
- 작업 10-I/J 요청 범위 완료. 기존 10-H 실패 복구/마감과 01/08/09 장비 게이트는 완료하지 않았다. 잔여 번호 4개, 작업 10 잔여 문자 H 1개. commit/push 없음. 이 인수인계서를 읽는 것만으로 후속 작업이나 업로드를 자동 재개하지 않는다.

## 과거 OTA 시험 빌드 및 구현 결과 (Vr1.0.1.2)

- 2026-08-31 VS2022 Release/x64 전체 Rebuild 성공: 경고 0, 오류 0. Protocol C++98 host test와 OTA uploader dry run도 성공했다.
- Main: `Vm1.0.9.0`, application 884,857 bytes, 배포 bin 885,216 bytes.
- Rod: `Vr1.0.1.2`, application 766,713 bytes, 배포 bin 767,072 bytes. OTA data frame 7,991개.
- 현재 배포 폴더는 `bin/release/x64/Vm1.0.9.0`, `bin/release/x64/Vr1.0.1.2`이다.
- 별도 보존된 배포용 `Vr1.0.1.0`은 OTA 수신기를 추가한 최초 설치 기준 image다. 같은 버전 표기의 legacy 원본과 동일한 코드로 취급하지 않는다.
- Main 중계기는 `DF_Main_RodOtaRelay.cpp/.h`, Rod 수신기는 `DF_Rod_Ota.cpp/.h`, PC 전송기는 `tools/upload-rod-ota.cmd` 및 `.ps1`이다.
- 무선 경로는 PC USB → Main → ESP-NOW → 등록·연결된 Rod 한 대다. Rod application만 전송하며 bootloader/partition/boot_app0는 유선 설치 대상이다.
- 서비스 경로는 `$OR<DFRO frame hex>%`다. 기존 `$DN R/r`는 Main에 Rod image가 기록되지 않도록 오류 3으로 거부한다.
- 최대 96-byte data, CRC32, session/sequence, ACK/retry 및 전체 SHA-256 검증을 사용한다. 검증 성공 후 비활성 OTA slot으로 부팅을 전환한다.
- OTA app slot은 각각 1,310,720 bytes, 서비스 image 상한은 1,250,000 bytes다. 전송 이어받기와 기능 이상 자동 rollback은 구현하지 않았다.

## 과거 정상 OTA 실물 확인 (복구 전)

- 2026-08-28 Main–Rod 무선 버전 왕복과 약 1초 진동을 시험했다. 사용자가 릴 연동 LED 점멸 및 실제 진동을 확인했다.
- 2026-08-31 사용자 승인으로 Main 중계 image와 Rod OTA 기준 image를 먼저 유선 설치했다. ROM bootloader `--no-stub`, 115200 baud 방식으로 네 영역 hash/verify를 확인했다. 전체 flash erase 및 페어링 재등록은 하지 않았다.
- 이어서 Rod `Vr1.0.1.2` application 767,072 bytes를 Main 경유 무선 전송했다. 모든 block/Finish ACK, `[OTA] Completed`, 종료 코드 0을 확인했다.
- 재부팅 후 Main 경유 버전은 `Vm1.0.9.0,Vr1.0.1.2`, Rod 직접 버전은 `Vr1.0.1.2`였다. 이는 해당 OTA 시험 직후 상태이며 현재 연결 포트를 고정값으로 가정하지 않는다.
- OTA 명령 호출 11:16:54.119부터 완료 출력 11:19:43.376(KST)까지 169.257초, 약 2분 49초였다. PC 준비·ACK·검증을 포함하고 앞선 유선 설치와 이후 버전 조회는 제외한다. 무선 전송만의 계측값은 아니다.
- HANDLE은 Rod 홀센서 GPIO18이다. 원시 입력 변화에서 오른쪽 버튼 LED(GPIO9, active-low)를 즉시 켜고 마지막 변화 200ms 후 원래 버튼 LED 상태로 복귀한다. R 버튼 해제 상태에서는 소등되며 연속 회전은 점등 시간을 연장한다.
- 사용자 확인: “LED 점등도 완료되었고, 소등도 확인했고, 연속 회전 중에는 계속 켜졌어”. 세 동작 모두 확인됐으며 200ms 정확도는 계측값이 아닌 코드 설정값이다.
- 확인 범위는 정상 OTA·재부팅·버전 응답·위 사용자 관찰이다. 모든 오류 조건이나 장시간 안정성까지 검증된 것으로 확대 해석하지 않는다.

## 이전 이주 문서 정리 이력

2026-08-31: 기존 인수인계서를 현재 정책·완료 결과 중심으로 갱신했다. 후속 작업 목록은 제외했다. 이번 변경은 이 문서에 한정되며 코드 수정, 재빌드, 업로드, commit/push는 수행하지 않았다. 이주 문서 작성 범위의 잔여 작업은 0개다.
