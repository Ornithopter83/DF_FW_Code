# 현재 개발환경 안내 (2026-10-02)

- TM 전용 VS2022: `DFTestModule.sln` (같은 csproj 사용).
- TM 전용 VS Code: `DFTestModule.code-workspace`, coreclr Debug 구성.
- 공통 Debug: `../tools/build-testmodule-debug.cmd`, 루트 `bin/testmodule/Debug/win-x64` DLL/PDB.
- 공통 Release 게시: `../tools/build-testmodule.cmd Release`, 루트 `bin/testmodule/Release` 버전 단일 EXE.
- SDK 8.0.419/초기 잠금과 Debug 빌드를 완료했다. Release NU1004 이후 공통 의존성 설정을 수정했으며 잠금 갱신/재빌드는 새 승인 대기다.
- 상세 절차: [IDE 인수인계](../docs/build/IDE_HANDOFF.md). 수정 전 Debug 빌드 성공, Release/수정 후 빌드 및 실제 디버깅·실행 확인은 남았다.
- 아래 기존 설명 중 외부 DF_OUTPUT 복사와 Debug 단일 EXE 배포는 현재 명령에 적용하지 않는다.

---

# DF TestModule

legacy/TM_V034의 장비 프로토콜을 기준으로 새로 구성한 독립 WinForms 프로그램이다. 원본 legacy는 변경하지 않는다.

- 관리 경로: testModule
- Target: .NET 8 Windows x64
- 배포 형식: self-contained single-file DFTestModule.exe
- Release 출력: bin/testmodule/Release/
- 화면: 모터 제어, 릴/IMU, LED, 입력/전원, 펌웨어 업데이트
- 상시 화면: 상단 연결/버전, 우측 수동 명령/로그

## 빌드와 게시

PowerShell에서 다음을 실행한다.

    .\tools\build-testmodule.cmd Release

배포에는 bin/testmodule/Release/DFTestModule_V041.exe 하나만 사용한다. 대상 PC에 .NET 런타임을 별도 설치할 필요가 없다.

## 자체 검사

    bin/testmodule/Release/DFTestModule_V041.exe --self-test

종료 코드 0이면 명령 생성, 수신 frame, LZ 압축, firmware ZIP 검증을 통과한 상태다.

## 펌웨어 ZIP

ZIP에는 이름에 DF_MAIN 또는 DF_ROD가 들어간 BIN 파일이 정확히 하나 있어야 한다. 루트의 선택적 manifest.json에는 target, version, size, sha256을 넣을 수 있다.

실제 COM 연결, 모터/릴/LED 동작과 펌웨어 업데이트는 대상 장비에서 확인해야 한다.
