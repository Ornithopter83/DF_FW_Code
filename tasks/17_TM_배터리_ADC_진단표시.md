# 작업 17

TM 배터리 상태와 ADC 진단 표시

## 목표와 범위

TM에서 배터리 퍼센트와 기존 충전/방전 상태 코드를 함께 표시하고, 우측에 Rod ADC 현재값·평균값·환산값을 보여준다. Rod→Main→TM 경로는 1초 간격으로 진단값을 전달한다. 기존 PID 26 배터리 프레임은 유지한다.

## 세부 작업

### A. Rod ADC 진단값과 Main 중계

- Rod 배터리 계산에서 clamp 전 환산 퍼센트를 보존한다.
- 새 Rod→Main PID 28 payload: `현재ADC,20회평균ADC,환산퍼센트,표시퍼센트`.
- Rod는 1초 주기로 전송하되 AP type이 TM일 때만 보낸다.
- Main은 TM AP에서만 PC `$31` 프레임으로 중계한다. 게임 프로그램 수신 흐름과 기존 `$12` 배터리 상태 보고는 유지한다.
- Main/Rod compile/link 빌드는 아직 실행하지 않았다. 사용자 사전 승인이 필요하다.

### B. TM 배터리 UI와 프로토콜 문서

- 퍼센트와 기존 상태 코드를 `99% (-3)`처럼 함께 표시한다. -3 변화 없음, -4 충전 중, -5 사용 중이며 -1/-2는 미확인·연결 끊김 상태다.
- 우측에는 현재 ADC, 20회 평균 ADC, clamp 전 환산값과 화면 표시값을 나란히 표시한다.
- 평균은 500ms 간격 샘플 20개(약 10초)이며, 2828~3546 ADC를 0~100%로 선형 변환하고 화면 표시값은 0~99%로 제한한다.
- 프로토콜 계약과 이 작업서를 갱신한다.

### C. TM 자체 포함 단일 EXE 게시

- 2026-10-13 사용자 승인 후 Release 단일 EXE 게시 성공, exit 0.
- 산출물 `bin/testmodule/Release/DFTestModule_V041.exe`, 83,115,838 bytes. staging과 최종 게시 폴더 모두 단일 EXE 파일을 포함한다.
- GUI 표시, 실제 COM 통신, 장비 확인은 별도 테스트이며 수행하지 않는다.

### D. Main/Rod 펌웨어 빌드

- 사용자 승인으로 `tools/build-main.cmd Release x64`와 `tools/build-rod.cmd Release x64`를 각각 성공 실행했다.
- Main `Vm1.0.10.0`: application 909,840 bytes, bin 경로 `bin/release/x64/Vm1.0.10.0/`에 대상 파일 4개.
- Rod `Vr1.0.2.0`: application 767,456 bytes, bin 경로 `bin/release/x64/Vr1.0.2.0/`에 대상 파일 4개.
- 이전 제한 실행 환경의 X: 매핑 거부 후 사용자 승인으로 제한 환경 밖에서 다시 실행했다. 실제 플래시 및 장비 검증은 미실행이다.

## 결과

- A/B 구현: 2026-10-13. 코드 편집 근거이며 펌웨어 빌드나 장비 확인을 뜻하지 않는다.
- C/D 빌드 결과는 `docs/build/TM_BATTERY_DIAGNOSTICS_BUILD_2026-10-13.md`에 기록한다.
- Main 및 Rod 펌웨어 모두 새 PID 28 진단 경로를 포함해야 한다. TM 단독으로는 ADC 진단값을 표시할 수 없다.

### F. 배터리 요청 반환값으로 표시 기준 통일

- 2026-10-13 수정: PID 28의 네 번째 값이 ADC 환산 후 `rest_lvl`을 사용해 PID 26의 실제 전송값과 달라질 수 있던 문제를 수정했다. 이제 Rod의 PID 26 payload와 같은 `restLvlStr` 값을 보고한다.
- TM 우측 문구를 `ADC 환산 → 요청 반환`으로 바꾸고, 요청 반환값은 좌측 배터리 퍼센트와 같다고 안내한다. ADC 환산 퍼센트는 측정 참고값이다.
- Main/Rod/TM 빌드, 테스트, 장비 확인은 이번 수정에서 실행하지 않았다. 새 변경분의 빌드에는 사용자 사전 승인이 필요하다.

### G. 기준값 통일 수정 빌드·검사 시도

- 2026-10-13 사용자 승인 후 Rod Release x64 빌드 1회, TM Release 단일 EXE 게시 1회, 공유 프로토콜 host test 1회를 실행했다.
- Rod 빌드는 `artifacts/build/DF_Rod/build.options.json` 정리 단계의 Access denied로 종료 코드 1.
- TM 게시 빌드는 기존 `artifacts/publish/TestModule/DFTestModule_V041.exe` 정리 단계의 Access denied로 종료 코드 1.
- 프로토콜 host test는 `artifacts/host-tests/DFProtocol_HostTest.exe` 링크 출력에서 Permission denied로 종료 코드 1.
- 지침에 따라 승인 범위를 넘는 재시도는 하지 않았다. 새 소스의 빌드 성공, host test 성공, 실제 TM 동작은 확인되지 않았다. 플래시 및 장비 시험도 미실행이다.
- 상세 실행 결과: `docs/build/TM_BATTERY_CANONICAL_BUILD_2026-10-13.md`.

### H. TM 배터리 진행률 표시

- 2026-10-13 수정: 기존 숫자 상태칸을 사용자 요청 형태의 배터리 게이지로 바꿨다. 채움량과 퍼센트는 `$12` 배터리 요청 반환값을 사용하며, 게이지 괄호에는 20회 평균 ADC를 표시한다.
- 게이지 아래 왼쪽/오른쪽 기준을 `0% (2828)` 및 `99% (3546)`로 표시하고, 오른쪽에는 현재 ADC와 평균 ADC를 표시한다. 충전/방전 상태 코드는 퍼센트 옆에 유지한다.
- 빌드, UI 실행, 통신 및 장비 확인은 미실행이다. 새 UI 수정은 미빌드 상태다.

## 진행

작업 17-A~F/H 소스 수정 완료. 초기 TM/Main/Rod Release 산출물은 생성했지만 F/H 후속 수정은 아직 빌드되지 않았다. G에서 시도한 Rod/TM 빌드와 protocol host test는 접근 권한 오류로 실패했으며 재시도는 승인 대기다. GUI·COM·프로토콜 통합·장비 확인 및 펌웨어 플래시는 미실행이다.
