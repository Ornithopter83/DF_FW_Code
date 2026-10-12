# TM IMU 영점·손잡이 표시 보정 결과

작성일: 2026-10-02

## 변경

- ImuOrientationView의 카메라 X=3.2를 0으로 변경했다. 0점 자세에서 FRONT(-Z) 축이 우측으로 투영되는 카메라 편향을 제거한다. 높이와 Z 위치는 유지한다.
- HandleRotationScale=1.30을 적용해 $14 수신 이벤트당 손잡이 표시 회전각을 18도에서 23.4도로 늘렸다.
- 기존 영점 계산, IMU/엔코더 원값과 통신·펌웨어·물리 출력은 변경하지 않았다.

## 승인 실행 결과

| 항목 | 횟수 | 결과 |
| --- | --- | --- |
| tools/restore-testmodule.cmd | 1회 | exit 0, 현재 프로젝트 기준 NuGet 잠금 재평가 성공 |
| tools/build-testmodule.cmd Release | 1회 | exit 0, 단일 EXE 게시 성공 |
| 실패 재시도 | 0회 | 수행하지 않음 |
| 테스트·장비 조작·실제 화면 확인 | 0회 | 미실행 |

Release 명령 내부에서는 잠긴 모드의 restore가 한 번 실행됐다. 명시 복원 후 packages.lock.json의 Git 내용 차이는 없다. 이전 NU1004 오류는 이번 Release 게시에서 발생하지 않았다.

## 실행 파일

- 경로: bin/testmodule/Release/DFTestModule_V041.exe
- 크기: 83,103,038 bytes
- SHA-256: 34A22BF1A112CB1EB5B2F8EC33334BF448B0FF46CDD31D33EC5E05A91F989EF1
- 로그: artifacts/tm-display-20261002/restore.log, release.log 및 각 exit 파일.

## 남은 확인

FRONT가 정확히 정면으로 보이는지와 손잡이 표시 회전량의 물리적 일치는 사용자 화면·장비 확인이 필요하다. EXE 실행과 자체 테스트는 이번 범위에서 수행하지 않았다.

작업 13 A/B는 수정 및 빌드 완료다. 작업 12-C는 현재 잠금 재평가와 Release 성공을 근거로 완료 처리한다. 작업 12-H에는 수정 후 TM Debug 빌드가 남고, I의 두 IDE 디버깅·경로 이동 확인은 미실행이다. 잔여 번호 1개(12), 잔여 문자 2개(H/I).

기존 백업 태그 backup/local-20261002-143949는 bc8e551을 보존한다. 이번 수정·빌드·문서 변경은 그 이후 로컬 작업이며 새 commit/push는 수행하지 않았다.
