# DF Firmware 스레드 이주 명령서

Updated: 2026-09-01

## 인수인계 요청

이 문서를 읽고 기존 작업의 정책과 확인된 상태만 인수인계한다. 사용자의 요청에 따라 후속 작업, 권장 순서 및 신규 작업 지시는 포함하지 않는다. 아래 참조 문서에 실행 계획이 있더라도 자동으로 재개하지 말고, 인수인계한 상태를 짧게 보고한 뒤 사용자의 새 지시를 기다린다.

## 작업 위치와 참조

- 저장소 루트: `C:\Projects\VS\DF_FW_Code\CodexManage`
- 원격 저장소: `https://github.com/Ornithopter83/DF_FW_Code.git`
- 읽는 순서: `AGENTS.md` → `DF_FIRMWARE_INTEGRATED_REFACTORING_PLAN.md` → `CurrentWork.md` → `PROJECT_COMMANDS.md` → `tasks/11_독립형_TestModule_프로그램.md`
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

## 최신 TestModule 상태 (2026-09-01)

- 독립형 TestModule은 testModule에 있고 표준 출력은 bin/testmodule/Release/win-x64/DFTestModule.exe 단일 파일이다. 루트 솔루션에 포함되지만 펌웨어 프로젝트와 참조 관계는 없다.
- 최초 MAIN 시리얼 연결은 릴 연결 시도와 함께 IMU 출력 OFF $080%를 보낸다.
- 업데이트 화면은 MAIN, ROD 무선, ROD 유선, ROD 등록 네 버튼이다. 폴더에서 대상 application BIN과 내부 버전을 검증한다.
- MAIN은 연결 해제를 확인한 뒤 $DN M; ROD 무선은 MAIN/ROD 연결 후 $OR DFRO OTA; ROD 유선은 팝업 포트 선택 후 별도 $DN R; 등록은 $3001% 후 ROD 왼쪽 버튼 장기 누름과 $3003~$3005 상태를 처리한다.
- LM JIG 과거 구현은 legacy/TM_V034/FishingDebugger/LM_JIG_Settings.cs 및 DFTMmainForm.cs에 있으나 #if(false) 상태다. 현재 활성 Main도 관련 설정 0 및 빈 분석 함수로 비활성이다.
- MAIN 업데이트에도 포트 선택 팝업을 적용했다. COM4가 존재하면 기본 선택한다. 상단 포트 목록도 COM4 우선이며 창 높이는 50px 늘어난 900px이다.
- MAIN 업데이트 진행률 100% 후 soft reset된 기존 COM 객체로 초기 명령을 보내던 timeout을 수정했다. 완료 후 포트를 닫고 최대 12초 재등장을 기다려 다시 열며, 최대 6초 동안 선택 BIN의 Vm 버전을 확인한다.
- 후속 재시험의 약 6초 실패는 32KiB 첫 블록과 abort가 각각 3초 write timeout된 문제로 확인했다. 실제 Vm1.0.8.0 BIN의 최대 payload는 31,427 bytes/2.73초였다. $DN 블록을 16KiB로 낮추고 WriteTimeout을 10초로 늘려 MAIN/ROD 유선에 적용했다.
- COM4와 COM3가 모두 ESP32-S3 USB 직렬 장치라 MAIN 업데이트 전에 $10%/Vm 응답으로 대상을 확인한다. 16KiB protocol frame은 유지하고 .NET SerialPort.Write를 1KiB씩 나눠 단일 대용량 write 정지를 방지했다.
- 후속 사용자 지시로 MAIN 사전 검사와 업데이트 후 자동 재연결·반복 버전 확인을 제거했다. 115200bps, 500ms write timeout, 3초 단일 ACK 대기이며 초기 연결은 첫 write 실패에서 팝업 없이 중단한다. 완료 뒤 연결은 사용자가 수동으로 수행한다.
- Release compile/self-test/단일 EXE 게시 성공. 71,755,983 bytes, SHA-256 C96566198AD6992A15CCEE8F68B4F4548E5CFDFE051010E2A606FF47FE03F31D. 장비 재시험은 수행하지 않았다. 작업 11은 I 장비 gate 1개가 남아 있다.
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










## 2026-09-01 MAIN 복구 업데이트 최신 상태

- MAIN 업데이트는 더 이상 $DN application OTA를 사용하지 않는다. 불완전 설치·부팅 반복 상태에서도 동작하도록 내장 esptool_V4.5.1.exe로 ROM bootloader 직접 기록한다.
- update.bat 기준으로 포트 선택 후 921600bps, default_reset/hard_reset, 0x0 bootloader, 0x8000 partitions, 0xe000 boot_app0, 0x10000 application을 한 번 기록한다. 일반 시리얼은 115200bps다.
- MAIN 폴더에는 DF_Main.ino.bootloader.bin, DF_Main.ino.partitions.bin, boot_app0.bin, DF_Main.ino.bin 네 파일이 필요하다.
- 최종 Release EXE: bin/testmodule/Release/win-x64/DFTestModule.exe, 78,441,820 bytes, SHA-256 9EE409FC59B56091C731C1F77F2A27C093C1FFC080C87262E5D1BE1928A0A56C. build/self-test 성공, 실제 flash는 미수행이다.


## 2026-09-01 TestModule 최신 보완

- MAIN 버튼에서 (복구) 표기를 제거했지만 내장 esptool 네 영역 기록 기능은 유지한다.
- 1초 연결 polling은 유지하며 $00%와 $1800% 반복 정상 프레임은 로그에 표시하지 않는다. 다른 $18 상태는 계속 표시한다.
- ROD 유선도 내장 esptool로 변경했다. ROD 폴더에 DF_Rod.ino.bootloader.bin, DF_Rod.ino.partitions.bin, boot_app0.bin, DF_Rod.ino.bin 네 파일이 필요하다. ROD 무선은 기존 방식이다.
- 최종 Release EXE: bin/testmodule/Release/win-x64/DFTestModule.exe, 78,442,332 bytes, SHA-256 D977F582EF97003B77E45E347DA3574935820768302EE1CD907E2F2A6BAEE7B0. build/self-test 성공, 실제 ROD flash는 미수행이다.

## 2026-09-01 $1800 처리 정정

- $1800%는 MAIN이 AP에 보내는 PC 종료 요청이다. TestModule은 수신과 송신을 모두 로그에 남기고 연결당 한 번 같은 $1800%로 응답한다. 숨김 필터는 제거했다.
- MAIN의 (복구) 표기는 제거했다. MAIN/ROD 유선은 모두 내장 esptool 네 영역 기록이며 ROD 무선만 기존 OTA다.
- 최종 EXE: bin/testmodule/Release/win-x64/DFTestModule.exe, 78,442,332 bytes, SHA-256 856148C7E06CE948B70F0BFCF80CE7345C8B0E7DE58414CECAC4F45BAC3E6248. 실제 ROD flash는 미수행이다.

## 2026-09-01 첫 연결 초기화 최신 상태

- 업데이트 후 첫 연결에서 ROD/IMU 상태가 누락된 원인은 포트 오픈 직후 초기 명령 8개를 연속 송신해 MAIN의 단일 rcved_flag 대기 버퍼에서 후속 명령이 유실된 것이다.
- 연결은 레거시와 같이 500ms 간격 3단계로 초기화한다. 각 단계 내부는 60ms 간격이며 초기화 완료 뒤 2초 polling을 시작한다.
- $1800%는 MAIN의 AP 종료 요청으로 계속 표시하고 연결당 한 번 응답한다.
- 최종 EXE: bin/testmodule/Release/win-x64/DFTestModule.exe, 78,444,892 bytes, SHA-256 42463DC0003174E03E43099B1D1F29B86F1190B2CFD018D4F478CBD6A41A4450. 실제 장비 첫 연결 검증은 미수행이다.
## 2026-09-01 첫 연결 로그 재분석 최신 상태

- `DFLOG[202609011503].txt`는 연결 직후 이전 `$2001/$2101/$1800`가 몰려 들어오고, TestModule의 자동 `$1800%` 응답 뒤 `$2211%` MAIN reset이 발생했음을 보여준다. 사용자의 업데이트 후 10초 대기 부족이 원인이 아니다.
- 포트를 연 직후 남은 입출력 버퍼를 비우고 수신 이벤트를 연결하며 frame decoder도 초기화한다.
- `$1800%`는 숨기지 않지만 실제 종료하지 않는 TestModule이 완료 ACK를 보내던 처리는 제거했다. 연결당 한 번 경고만 남긴다. 이 항목이 위의 `$1800%` 자동 응답 기록을 대체한다.
- ROD/IMU 상태 조회를 먼저 수행하고 두 연결 상태를 받거나 최대 2초 대기한 뒤 `$10%`를 한 번 보내므로 초기 `Vs99.99.99/Vi99.99.99` 고정을 방지한다.
- 최신 표준 EXE: bin/testmodule/Release/win-x64/DFTestModule.exe, 78,445,404 bytes, SHA-256 6F300C57328983138A3790F407CCA86F3E0C49645DFFA738428E998907D64819. build/self-test 성공, 실제 장비 재시험은 미수행이다.
