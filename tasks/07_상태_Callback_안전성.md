# 작업 07

상태와 Callback 안전성 개선

## 목표

물리 분리 후 전역 상태, 전역 객체 초기화, ESP-NOW callback과 고정 버퍼 경계를 명시적으로 안전하게 만든다.

## 현재 기준

- 헤더 정의 전역과 다수의 전역 상태가 단일 `.ino` 구조에 의존한다.
- Main/Rod 수신 callback이 128바이트 배열에 `data_len` 검증 없이 복사한다.
- callback 안에서 Arduino `String` 생성/변환이 수행된다.

## 관련 영역

- Main/Rod application state, hardware object ownership, ENow callbacks, scheduler

## 구현 원칙

- C++03 호환 구조체와 명시적인 owner를 사용한다.
- callback은 고정 버퍼 복사와 flag 설정만 담당하게 한다.
- 결함 수정은 한 항목씩 수행하고 각각 수동 검증한다.

## 세부 작업

### A. 헤더 저장 공간과 ODR 정리

### B. Main 상태와 hardware object 소유 위치 확정

### C. Rod 상태와 hardware object 소유 위치 확정

### D. ESP-NOW 수신 길이 및 종료문자 경계 검사

### E. Callback-to-loop 전달 구조 정리

### F. 초기화 순서와 critical section 검토

### G. 수동 부하/통신/장비 검증 게이트

## 진행

잔여 작업 7개 (A, B, C, D, E, F, G)

## 변경 금지

- 동적 메모리 기반 queue, 스마트 포인터, lambda callback을 도입하지 않는다.

## 완료 조건

- 수신 길이 초과가 차단되고 callback은 경량이며 전역 상태와 객체 소유 위치가 명확하다.

## 시험 방법

- 사용자가 연속 ESP-NOW 송수신, 비정상 길이 입력, 재연결 및 장시간 장비 시험을 수행한다.

## 결과

- 작업 수행 후 기록.

