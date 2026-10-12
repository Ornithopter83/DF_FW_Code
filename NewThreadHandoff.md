# DF Firmware 스레드 인수인계

Updated: 2026-10-02

## 현재 작업 — 16 AP/TCP 및 BoardWifiConnector

최신 완료: 승인된 재시도 1회에서 자체 포함 단일 EXE 게시 exit 0. bin/BoardWifiConnector/Release/BoardWifiConnector.exe, 161,859,788 bytes. 네이티브 라이브러리·기본 INI 양식 포함, Release PDB 제외. 최초 실행 시 connection.ini 생성. 기존 Main 성공과 함께 16-A/B/C 완료. 잔여 번호 2개(12,15), 문자 3개(12-H/I,15-B). 실행/정적/통신/장비/실제 종료·등록·플래시·commit/push 미실행. 기존 작업은 자동 재개하지 않는다. 아래 재게시 대기 기록은 이전 이력이다.

최신: 승인된 Main 빌드 1회 성공(네 파일, application BIN 909,696 bytes), Connector 명시 복원 1회 성공(packages.lock.json 생성), Release 게시 1회 CS8510 실패. SocketException 패턴을 Win32Exception 앞에 옮긴 소스 수정 후 재빌드는 미실행이다. Connector EXE는 없다. 다음 Connector Release 게시 1회는 새 사전 승인 필요. 16-C 및 전체 잔여 3번호/4문자를 유지한다. docs/build/BOARD_WIFI_BUILD_2026-10-02.md. 아래 미실행 설명은 구현 직후 이력이다.

16-A/B 소스 구현 완료. Main은 채널 1 AP+STA/TCP를 제공하고 게임 시리얼 연결을 우선한다. 게임 연결 부재/상실 시 Wi-Fi로 종료 요청을 전송하며 기존 응답·USB5V·2분 차단 상태에 합류한다. 루트 BoardWifiConnector는 단일 인스턴스 WinForms 트레이, 고정 INI, USB 최초 등록, DPAPI 토큰, 생존 신호와 자동 재연결, 실제 Windows 종료 접수 후 ACK를 제공한다. 사용자는 로그온 후 동작이면 충분하다고 확인했다.

NuGet 복원·잠금 생성·Main/Connector 빌드·정적/실행/장비 테스트·EXE 생성·등록/설치·플래시·실제 종료·commit/push는 미실행이다. 16-C는 새 승인 필요. 기본 AllowWindowsShutdown=false이며 실제 종료는 사용자 등록 후 true 설정/재연결이 필요하다. 기존 2분 강제 차단 조건은 유지된다.

작업서 tasks/16_BoardWifiConnector_AP_TCP.md, 안내 BoardWifiConnector/README.md. 변경 전 Main 네 파일은 deprecated/main/board-wifi-before-20261002에 보존했다. 전체 잔여 번호 3개(12,15,16), 문자 4개(12-H/I,15-B,16-C). 아래 이전 작업 상태는 각 시점의 이력이다.

## 최신 완료 — 작업 14 릴 그림 가려짐 (2026-10-02)

- 공통 픽셀 깊이 버퍼로 본체·손잡이·사진을 처리하고 숨은 테두리를 제외했다. 원본은 deprecated/testmodule/reel-rendering-before-depth-20261002에 보존한다.
- 이전 FRONT 정렬·손잡이 표시 1.3배·센서/통신/물리 출력 유지.
- TM Release 게시 1회 exit 0. 최신 bin/testmodule/Release/DFTestModule_V041.exe는 83,112,766 bytes. 작업 13의 기존 EXE를 교체했다.
- 실제 회전 화면·성능·장비와 별도 테스트는 미실행. 최신 변경 이후 Debug 및 IDE/이동 확인은 새 승인 대기다.
- 잔여 번호 1개(12), 잔여 문자 2개(H/I). 작업 14 A/B 완료.
- 결과: docs/build/TM_REEL_RENDERING_FIX_2026-10-02.md. 현재 변경은 기존 백업 태그 이후 로컬 상태이며 commit/push 미실행이다.
- 아래 이전 산출물·표시 수정 기록은 해당 시점의 기록이다.

## 현재 요청 — 디밍 옵션 구현 (2026-10-02)

작업 15-A 구현 완료: 디밍 패턴 1~6, 최초 6(Default), 우측 순환 색 이름 표시와 패턴값 전송을 분리했다. DeviceProtocol에서도 액션 3의 범위를 1~6으로 제한한다. 이번 요청에는 빌드·테스트 승인이 없으므로 15-B는 승인 대기다. 기존 작업 14 EXE에는 새 디밍 옵션이 포함되지 않는다. 잔여 번호 2개(12,15), 잔여 문자 3개(12-H/I,15-B). 작업서 tasks/15_TM_LED_디밍_패턴선택.md를 현재 우선한다. commit/push 미실행.

## 최우선 정책

- 이주/인수인계만 요청받으면 AGENTS.md와 이 문서만 읽고 짧게 보고한 뒤 기다린다. Git·소스 탐색·전체 문서 재독·작업 자동 재개를 하지 않는다.
- 빌드(compile/link), 테스트, 반복 검증, 추가 조사·탐색은 사용자 명시적 사전 승인 후 수행한다. 코드 수정 지시나 과거 승인은 포괄 승인이 아니다. 재시도와 범위 확장도 승인 범위를 확인한다.
- 명시적 요청 없이 commit/push/flash/배포/외부 시스템 변경을 수행하지 않는다. 플래시와 장비 시험은 사용자가 수동 수행한다.

## 현재 기준

- 저장소: `C:\Projects\AI-AGENTS\DF_FW_Code`.
- 연동 문서: `C:\Projects\AI-AGENTS\DF_Integration`.
- 현재 체크아웃은 `main`. 원격 백업 bc8e551 / `backup/local-20261002-143949`는 작업 13 수정 전 기준이다. 현재 표시 수정과 문서는 이후 로컬 변경이며 새 commit/push는 미실행이다.
- 백업 전 HEAD는 `0de2d0a`, 원격 main은 `c84be58`(2026-09-11)이었다. 원격 refs 조회에서 main만 존재했고 legacy 원본을 확인했다. 사용자 요청으로 commit/push와 백업 태그 지정 범위를 승인받았다. 자동 pull/merge/reset은 수행하지 않는다.
- 최신 백업 범위와 복원 안내: `docs/build/BACKUP_LOCAL_2026-10-02.md`. 과거 원격 비교: `docs/build/REMOTE_BACKUP_COMPARISON_2026-10-02.md`.
- Main/Rod 독립 펌웨어 + 공통 DFProtocol + .NET 8 Windows x64 WinForms TM. 활성 소스는 firmware/DF_Main, firmware/DF_Rod, libraries/DFProtocol, testModule이다.
- 현재 루트 DF_Firmware.sln에 세 프로젝트가 있으며 같은 csproj를 참조하는 TM 전용 .sln과 FW/TM VS Code workspace를 추가했다.
- 현재 소스 버전 기록은 Main Vm1.0.10.0 / Rod Vr1.0.2.0 / TM V041. 과거 성공 산출물을 현재 변경의 검증 결과로 사용하지 않는다.

## 마지막 완료 작업과 복구

- 사용자 요청: 복구 지점과 계획 작성, 개발환경 일괄 구현, 승인된 준비·빌드 실행을 거쳐 문서 최신화 및 로컬 버전 원격 백업 지정. 백업 지정은 작업 12의 빌드·IDE 확인 완료를 의미하지 않는다.
- 작업 12-A/B/C/D/E/F/G 구현·준비 완료. 작업 13-A/B에서 TM 표시 수정과 Release 게시 성공. 수정 후 Debug(H), IDE/이동(I), 복구 재현·장비 시험은 미실행이다.
- 보존 브랜치: `codex/restore-before-vscode-20261002-114314`, 커밋 기준점만 보존.
- 미커밋 백업: `artifacts/recovery/before-vscode-20261002-114314/`. 추적 변경 73개 + 미추적 파일 6개, 원본 79개 보존; 스테이징 변경/삭제 파일 없음.
- bundle, 원본 ZIP, index, patch, 파일별 SHA-256 manifest 생성 명령 exit 0. Git-ignored 로컬 toolchain/추가 산출물/개인 설정 및 외부 출력은 제외했다.
- 복원은 새 폴더에서 수행한다. 백업은 Git 제외 로컬 파일이므로 전달 시 별도 복사해야 한다.
- 복구 안내: `docs/build/RECOVERY_BEFORE_VSCODE_2026-10-02.md`.

## 계획과 다음 작업

- 목표: TM 분리 솔루션, VS2022/VS Code 같은 소스·빌드 명령·TM 디버깅, 루트 bin 상대경로 출력.
- 작업서: `tasks/12_VSCode_VS2022_호환_개발환경.md`.
- 상세 계획: `docs/build/VSCode_VS2022_COMPATIBILITY_PLAN.md`. 사용 절차: `docs/build/IDE_HANDOFF.md`.
- 잔여 번호 작업 1개(12), 작업 12 잔여 문자 2개(H/I). 작업 13 A/B 완료.
- SDK 8.0.419 / VS2022 17.14.37027.9 기준. 작업 13의 승인으로 잠금 재평가 1회와 TM Release 게시 1회가 성공하여 기존 NU1004는 이번 실행에서 발생하지 않았다. 현재 표시 수정 후 TM Debug와 I 확인은 미실행이며 새 승인 필요.
- 순서: B 경로 → C TM 솔루션/SDK → D Debug/Release → E FW VS Code → F TM VS Code → G 문서 → H 승인 빌드 → I IDE/이동 확인.
- TM Debug portable PDB/비최적화 설정과 Release 단일 EXE 게시를 분리했다. 고정 외부 복사를 제거하고 X: 일치/정리 범위 확인 코드를 작성했다. Main/Rod 출력 및 수정 전 TM Debug DLL/PDB는 확인했다. 작업 13에서 현재 Release 게시가 성공했다. 수정 후 TM Debug는 미실행이다. FW 하드웨어 중단점 디버깅은 별도 범위다.
- 후속 실행 지시를 받으면 로드맵 → CurrentWork → PROJECT_COMMANDS → 작업 12 순서로 읽는다. H/I의 실행은 별도 사전 승인이 필요하다.

## 보존할 기존 경계

- 사용자 미커밋 변경과 legacy/deprecated를 보존한다. 자체 FW 코드는 C++98/03, 고유 접두어, namespace/새 Variant 분기 금지.
- 기존 11-J 무선 업데이트 보완, 11-K LM JIG 구현은 완료 기록 유지. 물리 장비 검증 완료를 의미하지 않는다.
- 01 잔여/08-JK/09 전체/10-H/11-I는 사용자 제외 범위다. 새 IDE 계획에 자동 포함하지 않는다.
- 실패 IMU 보정 연구와 BLDC 추가 분석을 자동 재개하지 않는다. 실패 연구 보관 브랜치는 기존 기록으로 유지하며 자동 병합·삭제하지 않는다.
- TM의 통신/업데이트/장비 기능과 Main/Rod 동작·타이밍은 개발환경 정리에서 변경하지 않는다.

## 이번 실행 근거

상세 결과: docs/build/IDE_COMPATIBILITY_RUN_2026-10-02.md. 로그/hash: artifacts/ide-compat-20261002/. 초기 승인된 네 빌드를 각각 한 번 실행했다. 이후 작업 13의 새 승인에서 NuGet 잠금 재평가와 TM Release 게시를 각각 한 번 실행해 성공했다. 수정 후 Debug·IDE 확인은 별도 승인 필요하다.

## 2026-10-02 백업 기준

- 태그: `backup/local-20261002-143949`. 동일 커밋을 origin/main과 백업 태그에 보존하는 기준이다.
- 소스·공통 IDE 설정·SDK/초기 NuGet 잠금·문서·필수 ZIP/esptool과 기존 추적 산출물 변경을 포함한다.
- toolchain, 루트 bin, artifacts, 개인 설정 및 `.vscode/arduino.json`은 원격 백업에 포함하지 않는다. 기존 79파일 작업 전 ZIP은 로컬 보존을 유지한다.
- 이 문서 작성 및 Git 보존 과정에서 추가 빌드·테스트·장비 조작은 실행하지 않는다. C/H/I 잔여 3개는 유지한다.

## 마지막 표시 수정 결과 (2026-10-02)

ImuOrientationView 카메라 X=0으로 FRONT 기준축 투영을 정렬하고 표시 스텝을 18*1.30=23.4도로 변경했다. Release EXE는 bin/testmodule/Release/DFTestModule_V041.exe, 83,103,038 bytes. 센서·프로토콜·FW·물리 출력은 변경하지 않았다. 실제 화면·장비 확인, 자체 테스트, commit/push는 미실행이다. 결과: docs/build/TM_DISPLAY_CORRECTION_2026-10-02.md. 과거 실패 IMU 연구를 재개한 작업이 아니다.
