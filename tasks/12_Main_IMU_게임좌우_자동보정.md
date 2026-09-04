# 작업 12

Main IMU 게임 좌우 자동 보정

## 목표

고객의 수동 0점 보정 없이 충전 중 정지 상태에서 자이로 바이어스를 보정하고, 게임 중 Yaw 회전을 가상 Roll로 변환해 기존 게임 `$09` Roll 입력에 반영한다.

## 완료 작업

- A. Main 충전 정지 보정·Yaw 가상 Roll·게임 출력·Vm1.0.11.0 빌드 (완료: 2026-09-04)

## 진행

잔여 작업 0개

## 2026-09-04 A 결과

- `DF_Main_ImuGameControl` C++03 모듈을 추가해 `$14` IMU payload의 Roll/Pitch/Yaw/GX/GY/GZ를 처리한다.
- 기존 배터리 잔량 4% 이상 상승 판정이 충전 시작을 알리면 자이로 크기 3deg/s 이하 상태를 3초 확인하고 2초 동안 GX/GY/GZ 바이어스를 평균한다. 움직임이 감지되면 정지 확인부터 다시 시작한다.
- IMU 출력이 꺼져 있으면 보정 동안만 켜고 완료 또는 충전 종료 시 다시 끈다. 게임 프로그램 시작도 충전 상태를 종료한다.
- 게임 AP에만 Z축 자이로의 바이어스와 2deg/s dead zone을 적용해 가상 Roll을 만든다. 감도 0.65, 최대 ±30도, 정지 시 0.9초 감쇠를 사용하며 현재 Roll과 합친 값을 ±45도로 제한해 `$09` 첫 Roll 필드에 넣는다.
- TM AP에는 원본 `$09` payload를 유지한다. 충전 자세의 Roll/Pitch는 게임 영점으로 사용하지 않는다.
- Main 버전을 `Vm1.0.11.0`으로 변경했다.
- `tools/build-main.cmd` Release clean build 성공: application 사용량 891,105 bytes, 동적 메모리 153,848 bytes. 출력은 `bin/release/x64/Vm1.0.11.0/`의 application 891,472 bytes, bootloader 14,032 bytes, partitions 3,072 bytes, boot_app0 8,192 bytes다.
- 프로토콜 테스트, 플래시 및 실제 충전·게임 장비 검증은 수행하지 않았다.

