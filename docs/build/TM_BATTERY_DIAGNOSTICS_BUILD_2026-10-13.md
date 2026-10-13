# TM 배터리 ADC 진단 UI 게시 결과

날짜: 2026-10-13

## 실행

- 진단 결과 staging EXE는 배타적 읽기 열기가 가능했고 ACL에 Modify가 있어, 파일 잠금/명시적 ACL 거부가 원인으로 보이지 않았다. `X:`도 매핑되지 않은 상태였다. 제한된 실행 환경이 staging 정리와 SUBST를 거부하는 것으로 판단했다.
- 제한된 실행 환경의 TM 게시 3회는 모두 staging 정리 접근 거부로 실패했다. Main/Rod Release도 각 1회 `X:` 매핑 접근 거부로 실패했다.
- 이후 사용자 승인 아래 제한 실행 환경 밖에서 TM 게시와 Main/Rod Release를 각각 1회 실행했고, 세 결과 모두 exit 0이다. 최종 산출물은 아래 경로에 생성됐다.

### TM Release 단일 파일 게시

- 명령: `tools/build-testmodule.cmd Release`
- 출력: `bin/testmodule/Release/DFTestModule_V041.exe` — **83,115,838 bytes**.
- 확인: 게시 폴더와 staging 폴더 각각에 `DFTestModule_V041.exe` 단일 파일이 있다.

### Main Release 펌웨어

- 명령: `tools/build-main.cmd Release x64`
- application: `bin/release/x64/Vm1.0.10.0/DF_Main.ino.bin` — **909,840 bytes**.
- bootloader 14,032 bytes, partitions 3,072 bytes, boot_app0 8,192 bytes.
- Arduino 결과: program 909,469 bytes (69%), global variables 155,788 bytes (47%).

### Rod Release 펌웨어

- 명령: `tools/build-rod.cmd Release x64`
- application: `bin/release/x64/Vr1.0.2.0/DF_Rod.ino.bin` — **767,456 bytes**.
- bootloader 14,032 bytes, partitions 3,072 bytes, boot_app0 8,192 bytes.
- Arduino 결과: program 767,089 bytes (58%), global variables 45,520 bytes (13%).

펌웨어 flash, GUI/COM, 프로토콜 및 장비 시험은 실행하지 않았다. 추가 빌드/재시도도 하지 않았다.

## 재개 조건

작업 17 A~E 구현 및 세 Release 산출물 생성을 완료했다. 다만 GUI/COM/장비에서의 동작, 펌웨어 flash, Main/Rod 통합 통신 확인은 수행하지 않았으므로 실제 장비 동작까지 확인한 것은 아니다.
