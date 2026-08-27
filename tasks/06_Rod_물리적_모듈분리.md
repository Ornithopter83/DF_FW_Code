# 작업 06

Rod 펌웨어 물리적 모듈 분리

## 목표

`DF_Rod.ino`의 정상/설정 모드, 통신, IMU, 입력, 배터리 및 전원 처리를 기능별 C++98/03 모듈로 분리한다.

## 현재 기준

- `DF_Rod.ino`는 3,065줄이며 여러 장치와 mode 처리가 결합되어 있다.

## 관련 영역

- 향후 `firmware/DF_Rod/DF_Rod.ino`, `firmware/DF_Rod/src/`

## 구현 원칙

- 현재 최신 Rod 하드웨어만 지원한다.
- IMU UART와 IMU application 정책을 구분한다.
- setup/normal mode 동작 순서를 보존한다.

## 세부 작업

### A. Utility와 PC setup protocol 분리

### B. Rod registration 분리

### C. Battery monitor 분리

### D. Button/Encoder input control 분리

### E. IMU service 분리

### F. Power/Sleep 및 Main communication 분리

### G. Scheduler와 Rod application entry 분리

### H. 단계별 수동 빌드와 주요 smoke 게이트

- 각 문자 이동 뒤 Codex가 build를 실행한다.
- 입력, IMU, 등록, 통신 및 mode 경계에서는 장비 smoke test도 요청한다.

## 진행

잔여 작업 8개 (A, B, C, D, E, F, G, H)

## 변경 금지

- IMU 명령 형식, button/encoder 의미, registration 및 sleep sequence를 파일 이동과 함께 바꾸지 않는다.

## 완료 조건

- `.ino`가 초기화와 상위 process 중심으로 축소되고 각 장치/서비스 경계가 분리된다.

## 시험 방법

- 모든 문자 작업에 사용자 수동 compile/link 결과가 필요하며 관련 장비 smoke 결과가 필요하다.

## 결과

- 작업 수행 후 기록.
