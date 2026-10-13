# TM 배터리 요청 반환 기준 통일 빌드 기록

Updated: 2026-10-13

## 범위

TM 우측 진단의 네 번째 퍼센트를 Rod PID 26의 실제 배터리 payload와 같게 수정한 뒤 사용자 승인으로 Rod Release x64 빌드, TM Release 단일 EXE 게시 및 공유 프로토콜 host test를 각 1회 시도했다.

## 결과

| 작업 | 결과 | 오류 |
| --- | --- | --- |
| `tools/build-rod.cmd Release x64` | 실패, exit 1 | 기존 `artifacts/build/DF_Rod/build.options.json` 정리 시 Access denied |
| `tools/build-testmodule.cmd Release` | 실패, exit 1 | 기존 `artifacts/publish/TestModule/DFTestModule_V041.exe` 정리 시 Access denied |
| `tools/test-protocol.cmd` | 실패, exit 1 | `artifacts/host-tests/DFProtocol_HostTest.exe` 링크 출력 시 Permission denied |

승인된 각 명령은 한 번만 실행했다. 저장소 사전 승인 지침에 따라 재시도하지 않았다. 그러므로 이 수정의 compile/link 성공과 host test 성공은 확인되지 않았다. TM GUI/COM, Rod/Main 통신, 장비 동작 및 플래시는 수행하지 않았다.

## 현재 판정

- 소스 수정: 완료.
- 빌드 및 host test: 권한 오류로 미완료.
- 실제 배포 파일이 이번 소스를 반영하는지: 확인되지 않음.
- 다음 절차: 잠긴 출력 파일을 사용하는 프로세스를 정리하거나 출력 경로 권한 문제를 해결한 뒤, 새 사전 승인으로 필요한 명령을 다시 실행한다.
