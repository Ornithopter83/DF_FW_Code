# Active firmware source layout

Updated: 2026-08-27

## Source ownership

```text
firmware/
├─ DF_Main/
│  ├─ DF_Main.ino                 # setup/loop delegation only
│  └─ src/
│     ├─ DF_Main_Application.cpp  # ordered unity implementation owner
│     ├─ modules/
│     │  ├─ Foundation
│     │  ├─ Communication
│     │  ├─ LedApplication
│     │  ├─ ApplicationCommands
│     │  ├─ DeviceCheck
│     │  ├─ LmWire
│     │  ├─ GameControl
│     │  ├─ Diagnostics
│     │  └─ Scheduler
│     └─ existing device drivers
└─ DF_Rod/
   ├─ DF_Rod.ino                  # setup/loop delegation only
   └─ src/
      ├─ DF_Rod_Application.cpp   # ordered unity implementation owner
      ├─ modules/
      │  ├─ Foundation
      │  ├─ Communication
      │  ├─ SetupProtocol
      │  ├─ BatteryMonitor
      │  ├─ PowerAndImu
      │  ├─ Registration
      │  └─ SchedulerAndInput
      └─ existing device drivers
```

대형 기존 `.ino`는 Arduino가 자동 생성한 함수 선언과 기존 정의 순서를 보존한 상태로 기능별 `.inc` 구현 파일에 물리 분리했다. 이 파일들은 각 Application `.cpp`에서 정해진 순서로만 include하며 개별 번역 단위로 컴파일하지 않는다. 빌드 없는 대규모 이동에서 전역 상태와 선언 순서를 바꾸지 않기 위한 unity 경계다.

향후 독립 `.cpp/.h` 번역 단위로 전환하려면 공유 전역을 명시적 상태 구조체 또는 `extern` 계약으로 먼저 정리하고, 각 이동마다 빌드 및 장비 게이트를 수행해야 한다. 이번 작업에서는 사용자의 명시적 지시에 따라 해당 검증과 독립 번역 단위 전환을 수행하지 않았다.

## Preservation

- 전체 변경 전 원본: `legacy/Vm1.0.9.0`, `legacy/Vr1.0.1.0`
- 활성 코드에서 제거한 선택 분기와 이전 진입 파일: `deprecated/main`, `deprecated/rod`
- 활성 펌웨어는 `legacy/` 또는 `deprecated/`를 include하거나 빌드하지 않는다.
