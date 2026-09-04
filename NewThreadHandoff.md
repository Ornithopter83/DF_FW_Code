# DF Firmware 스레드 인수인계

Updated: 2026-09-04

## 최우선 정책

- 빌드(compile/link), 테스트, 반복 검증, 추가 조사·탐색은 무조건 사용자 명시적 사전 승인 후 수행한다. 코드 수정 요청이나 과거 승인을 포괄 승인으로 해석하지 않는다.
- 필요하면 목적과 범위를 짧게 설명하고 기다린다. 승인 범위를 넘는 재시도도 다시 승인받는다.
- 다른 문서의 자동 빌드·검증 규정보다 이 정책이 우선한다. 실행하지 않은 것은 미실행으로 기록한다.
- 이주 시 AGENTS.md와 이 문서만 읽고 현재 상태를 짧게 보고한 뒤 사용자 지시를 기다린다. 소스/Git 탐색, 전체 문서 재독, 과거 결과 재검증, 작업 자동 재개는 하지 않는다.
- 답변과 문서는 간결하게 유지한다. 불필요한 반복 읽기와 조사를 하지 않는다.

## 현재 상태

- 저장소: C:\Projects\VS\DF_FW_Code\CodexManage
- 사용자 지정 범위는 11-J 무선 업데이트 보완과 11-K LM JIG 실제 제어였다. 두 항목은 구현 완료로 기록되어 있으며 활성 구현 잔여 작업은 0개다.
- 기존 01/08/09/10-H/11-I 검증·마감은 사용자 요청으로 범위에서 제외했다. 자동으로 되살리지 않는다.
- 11-J: 무선 업데이트 시작 경고 제거, 전송 후 MAIN $00% polling 재개, ROD 재연결 대기 및 버전 조회, 재연결 여부를 포함하는 완료 팝업. 기존 96-byte payload/ACK 방식 유지.
- 11-K: DF_CONFIG_LMJIG 런타임 구성에서 LM 모터 및 $07 명령 제어 구현. $070/$071/$072 직접 제어, $0750~$0754 위치 제어, 센서 도달 또는 3초 제한에서 정지. 물리 장비 동작은 미검증이다.
- MAIN/ROD 유선 업데이트는 단일 EXE에 내장한 esptool_V4.5.1.exe로 네 영역을 기록한다. 일반 시리얼 115200bps, 유선 flash 설정 921600bps. ROD 무선은 MAIN 경유 OTA다.
- $1800%는 MAIN→AP 종료 요청이다. TestModule은 로그를 숨기지 않으며 자동 완료 ACK는 제거했다. 사용자는 약 2분 후 메시지가 사라지고 $00% polling만 유지되는 관찰을 정상으로 판단했다. 추가 분석을 자동 재개하지 않는다.
- BLDC는 사용자 확인으로 해결 완료. 최신 PWM 반전 처리를 유지하고 미해결 이슈로 되살리지 않는다.
- 소스 버전은 Main Vm1.0.11.0 / Rod Vr1.0.2.0. firmware-versions 도구가 소스 Version.cpp에서 출력 폴더 버전을 읽는다.
- TM 소스 버전은 사용자 지정 `V041`이며 별도 지시 전까지 변경하지 않는다. 모터 화면은 모두 왼쪽 정렬이며 BLDC는 방향·Duty·시간을 각각 별도 행으로 표시한다. 출력·정지와 동시 출력도 왼쪽 정렬이고 정지는 Duty 0 명령이다. 충전 기준 자이로 보정과 게임 좌우 표시도 유지한다. Visual Studio `FolderProfile`로 V041 단일 EXE를 직접 게시할 수 있으며 이번 변경의 빌드·게시·화면 검증은 미실행이다.
- Main `Vm1.0.11.0`에는 같은 자이로 보정과 가상 Roll 알고리즘을 C++03 모듈로 적용했다. 기존 배터리 4% 이상 상승 판정에서 충전 보정을 시작하고 필요하면 IMU를 임시로 켠다. 게임 AP의 `$09` 첫 Roll 필드만 변환하며 TM에는 원본을 유지한다. Release clean build는 성공했고 실제 장비 검증은 미실행이다.

## 마지막 작업에서 기록한 결과 — 이번 이주에서 재검증하지 않음

- MAIN clean build 성공: bin/release/x64/Vm1.0.10.0/DF_Main.ino.bin, 888,768 bytes.
- TestModule Release build/self-test 성공: bin/testmodule/Release/win-x64/DFTestModule.exe, 78,445,404 bytes.
- Protocol host test 성공. 위 결과는 사전 승인 정책 변경 전에 수행한 기록이다.
- 해당 작업에서 flash 및 물리 LM JIG 시험은 수행하지 않았다. 구현 완료를 장비 검증 완료로 해석하지 않는다.
- 기존 사용자 변경을 보존한다. 명시적 요청 없이 commit/push/flash/배포하지 않는다.

## 후속 작업이 지시된 경우의 참조

- 현재 진행 및 결과: CurrentWork.md
- 전체 계획: DF_FIRMWARE_INTEGRATED_REFACTORING_PLAN.md
- 작업 상세: tasks/11_독립형_TestModule_프로그램.md
- 명령: PROJECT_COMMANDS.md — 명령이 기록되어 있어도 실행에는 사전 승인이 필요하다.
- TestModule 소스: testModule/ (app 경로 사용 안 함), 출력: bin/testmodule/
- 활성 펌웨어: firmware/DF_Main/, firmware/DF_Rod/; 공유 프로토콜: libraries/DFProtocol/
- 자체 펌웨어 코드의 C++98/03, namespace 금지 등 기존 작성 정책은 AGENTS.md를 유지한다.
