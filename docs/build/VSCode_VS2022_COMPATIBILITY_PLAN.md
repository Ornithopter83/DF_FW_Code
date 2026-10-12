# TM 분리 및 VS Code / VS2022 호환 개발환경 계획

## 최신 상태 — 표시 수정 후 Release 성공 (2026-10-02)

작업 13에서 카메라 기준을 정면으로 조정하고 손잡이 표시 회전량 1.3배를 적용했다. 새 승인에 따른 NuGet 잠금 재평가 1회와 TM Release 게시 1회가 모두 성공해 기존 NU1004는 이번 실행에서 발생하지 않았다. 실행 파일: `bin/testmodule/Release/DFTestModule_V041.exe` (83,103,038 bytes). 수정 후 Debug 빌드·두 IDE/경로 이동·실제 화면/장비 확인은 미실행이다. 잔여 문자 12-H/I 2개. [상세 결과](TM_DISPLAY_CORRECTION_2026-10-02.md).

아래 기존 실행 기록은 각 시점의 상태다. 현재 실패·잠금 대기 설명은 위 최신 결과를 우선한다.

작성일: 2026-10-02
상태: A/B/D/E/F/G 구현 완료 / C의 SDK·잠금 준비 및 H/I 실행 승인 대기
사용 절차: `docs/build/IDE_HANDOFF.md`
작업서: `tasks/12_VSCode_VS2022_호환_개발환경.md`
복구 기록: `docs/build/RECOVERY_BEFORE_VSCODE_2026-10-02.md`

## 1. 목표

VS2022에서 수정한 같은 소스와 프로젝트를 인수자가 VS Code에서 열어 편집·빌드하고, TM은 소스 중단점으로 디버깅할 수 있게 한다. 인수자는 문서에 적힌 고정 버전의 도구를 준비한 뒤 저장소 전용 workspace를 연다.

| 사용자 목표 | 완료 시 관찰할 결과 |
| --- | --- |
| 1. TM 분리 솔루션 | `testModule/DFTestModule.sln`에서 TM만 열고 빌드·디버깅 가능 |
| 2. VS Code / VS2022 호환 | 같은 소스·csproj·공통 빌드 스크립트·구성·출력 위치 사용; TM 중단점·단계 실행 가능 |
| 3. 프로젝트 루트 bin 상대경로 | 폴더를 옮겨도 루트 `bin/`에 결과 생성; 고정 외부 폴더 자동 복사 제거 |

펌웨어의 편집·빌드 호환을 이번 범위에 포함한다. ESP32-S3의 JTAG/USB 하드웨어 중단점 디버깅은 장비·OpenOCD·빌드 설정을 별도로 정해야 하므로 이번 완료 조건에 포함하지 않는다.

## 2. 확인된 현재 상태

확인 방법은 저장소 문서·Git 상태·프로젝트/스크립트 읽기다. 현재 소스에 대한 새 빌드나 동작 검증 결과가 아니다.

| 영역 | 현재 상태 | 필요한 변경 |
| --- | --- | --- |
| Git 기준 | `main`, HEAD `0de2d0a`, 당시 origin/main보다 1커밋 앞섬; 미커밋 변경 존재 | 기존 변경을 포함한 별도 백업 완료; 구현은 문자 단위로 진행 |
| 통합 솔루션 | 루트 `DF_Firmware.sln`에 Main, Rod, TM 포함 | 같은 TM csproj를 참조하는 독립 .sln 추가 |
| 펌웨어 빌드 | VS2022 NMake가 `tools/build-main.cmd`, `build-rod.cmd` 호출 | VS Code도 같은 스크립트 호출 |
| FW 도구 | 프로젝트 로컬 Arduino CLI/Core, `X:` 짧은 경로 사용 | 현재 저장소를 가리키는지 확인하고 충돌 시 중단 |
| TM | .NET 8 WinForms, Windows x64, System.IO.Ports 8.0.0 | 프레임워크와 프로그램 동작 유지; 개발/게시 설정 정리 |
| TM 디버그 | csproj 및 게시 profile에 DebugType=none, DebugSymbols=false | Debug에서 PDB 생성; Release 단일 EXE 게시 분리 |
| TM profile | FolderProfile 무조건 지정, profile에 Release/심볼 제거 설정 | 일반 Debug 빌드에 게시 profile이 개입하지 않게 제한 |
| TM 빌드 스크립트 | Debug/Release 모두 publish; V041 파일명 중복 고정 | Debug 개발 빌드 명령 추가, 게시 버전은 csproj에서 도출 |
| 출력 | 루트 bin 사용과 고정 `C:\Projects\AI-AGENTS\DF_OUTPUT` 복사 병행 | 루트 bin만 기본 출력으로 사용 |
| FW 코드 분석 | VS2022 공통 props·분석 전용 헤더 존재 | VS Code Main/Rod 분석 설정 추가; 실제 컴파일 옵션 보존 |

기존 2026-09-09 문서의 HEAD/upstream 기록은 현재 Git 상태와 달랐다. 당시 기록은 이력으로 보존하고 현재 기준은 위 실측 값으로 갱신한다. origin/main 내용이나 원격 최신 상태를 확인한 것으로 해석하지 않는다.

## 3. 공통 구조와 역할

```text
DF_FW_Code/
├─ DF_Firmware.sln                    기존 Main/Rod/TM 통합 작업
├─ DF_Firmware.code-workspace         새 FW용 VS Code 진입점
├─ global.json                       두 솔루션·CLI 공통 SDK 기준
├─ firmware/DF_Main/                  두 IDE가 같은 파일 편집
├─ firmware/DF_Rod/
├─ libraries/DFProtocol/
├─ vs/                               VS2022 NMake·IntelliSense
├─ testModule/
│  ├─ DFTestModule.csproj             기존 유일한 TM 프로젝트
│  ├─ DFTestModule.sln                새 TM 전용 VS2022 진입점
│  ├─ DFTestModule.code-workspace     새 TM 전용 VS Code 진입점
│  └─ packages.lock.json             NuGet 의존성 잠금
├─ .vscode/                          FW 명령·C++ 분석 설정
├─ testModule/.vscode/                TM 명령·디버거 설정
├─ tools/                            공통 빌드·개발 설정 생성
├─ artifacts/                        중간 출력·생성한 분석 자료
└─ bin/                              배포와 개발 실행 결과
```

위 구조는 구현 목표다. workspace와 TM 솔루션을 구현했다. 루트 global.json(8.0.419)과 초기 NuGet lock을 생성했다. Release 잠금 불일치 수정 후 lock 재평가는 새 승인 대기다.

- TM 프로젝트를 복제하지 않는다. 독립 .sln과 루트 .sln이 같은 `DFTestModule.csproj`를 참조한다.
- 솔루션과 workspace는 IDE 진입점이다. 빌드 기준은 기존 스크립트와 프로젝트 파일에 둔다.
- TM 전용 솔루션/작업에서는 Main/Rod 빌드를 호출하지 않는다. 통합 솔루션의 기존 선택지는 유지한다.
- TM은 Windows용 WinForms를 유지한다. VS Code 전환을 Linux/macOS 실행 호환으로 해석하지 않는다.
- WinForms 소스 편집·빌드·디버깅은 VS Code에서 준비하며, 시각적 Designer 작업은 VS2022 사용 절차를 문서화한다. 현재 MainForm은 코드로 화면을 구성한다.

## 4. 출력 경로 계약

Configuration 표기는 기존 FW 소문자, TM Debug/Release를 유지한다. 두 IDE와 스크립트에서 각 경로를 정확히 일치시킨다.

| 결과 | 목표 경로 | 용도 |
| --- | --- | --- |
| Main FW | `bin/<debug 또는 release>/x64/<Vm버전>/` | application, bootloader, partitions, boot_app0 4개 |
| Rod FW | `bin/<debug 또는 release>/x64/<Vr버전>/` | 같은 4개 파일 |
| TM Debug | `bin/testmodule/Debug/win-x64/` | 실행 파일·DLL·portable PDB·의존 파일로 개발 실행 |
| TM Release 게시 | `bin/testmodule/Release/DFTestModule_<TmVersion>.exe` | 기존 단일 EXE 배포 경로 유지 |
| 빌드 중간물 | `artifacts/` 및 프로젝트 obj | 배포 폴더와 분리; Git 제외 |

- Main/Rod 버전은 기존 Version.cpp 읽기를 유지한다. TM 파일명은 csproj의 TmVersion/AssemblyName에서 도출하고 스크립트·launch 설정에 V041을 별도 관리하지 않는다.
- `.cmd`는 `%~dp0`에서 저장소 루트를 구한다. PowerShell은 `$PSScriptRoot`, MSBuild는 프로젝트/설정 파일 위치를 사용한다.
- `$(SolutionDir)`와 터미널의 현재 디렉터리에 출력 경로를 의존시키지 않는다. TM 독립 솔루션을 열어도 같은 루트 bin을 사용해야 한다.
- 고정 외부 출력 경로 자동 생성·복사를 기본 흐름에서 제거한다. 별도 외부 배포가 필요하면 추후 명시적 명령으로 추가한다.
- 게시 profile의 폴더 정리는 해당 TM 게시 폴더 안으로 제한한다. clean은 대상 구성/버전 경로가 루트 bin/artifacts 안에 있는지 확인한 뒤 수행한다.
- TM 실행 시 필요한 리소스와 내장 esptool은 기존 방식으로 유지한다. exe를 다른 폴더로 옮겨도 특정 개발 PC 경로를 요구하지 않아야 한다.
- 이미 존재하는 외부 출력이나 과거 bin을 삭제·이동하지 않는다.

`X:`는 컴파일러의 긴 경로 대응 수단이다. 출력의 외부 고정 경로와 구분해 유지한다. 다른 저장소를 가리키는 X:를 자동으로 덮어쓰지 않고 오류로 안내한다. 새 위치에서 저장소 루트와 매핑을 정확히 연결하는 절차를 마련한다.

## 5. TM 개발·디버깅·게시

| 항목 | Debug 개발 | Release 게시 |
| --- | --- | --- |
| 실행 준비 | `dotnet build`, 공통 Debug 스크립트 | 기존 공통 publish 스크립트 |
| 심볼 | DebugType=portable, DebugSymbols=true | 기존 정책에 맞춰 심볼 제외 |
| 최적화 | Optimize=false | 배포 설정 사용 |
| 단일 EXE | 일반 개발 출력으로 실행 | self-contained / win-x64 / single-file 유지 |
| 게시 profile | 적용하지 않음 | FolderProfile 명시 적용 |
| IDE 연결 | VS2022 시작 프로젝트 / VS Code coreclr | 결과 EXE를 배포 자료로 사용 |

1. csproj의 공통 영역에서 모든 구성에 적용되는 심볼 제거·단일 EXE 게시 옵션을 정리한다.
2. Debug 전용 PropertyGroup에서 개발 출력과 심볼을 지정한다. FolderProfile이 이를 덮어쓰지 않게 한다.
3. `tools/build-testmodule-debug.cmd`를 추가한다. VS2022와 VS Code의 개발 실행은 같은 프로젝트/Configuration/Platform/RuntimeIdentifier/OutputPath를 사용한다.
4. 기존 `tools/build-testmodule.cmd Release`는 게시 명령으로 유지하고 버전·출력 경로 중복값을 제거한다. Debug 게시 옵션을 남기는 경우 개발 디버깅과 이름·명령을 분명히 구분한다.
5. VS Code는 Microsoft C# 확장과 명시적 coreclr launch 설정을 기본으로 한다. C# Dev Kit의 자동 솔루션 인식 여부만으로 WinForms 실행을 보장하지 않는다.
6. VS Code의 DLL 경로·TM 버전은 csproj 기준으로 설정 생성 스크립트가 도출한다. 생성된 launch 설정과 실제 Debug 결과가 일치해야 한다. 버전 변경 시 같은 준비 명령으로 재생성한다.
7. VS2022의 시작 디렉터리와 VS Code cwd를 맞춘다. UI/시리얼/업데이트 코드와 타이밍은 환경 전환을 위해 변경하지 않는다.

정확한 .NET SDK 패치 버전은 구현 때 실제 설치 SDK와 VS2022 버전의 지원 관계를 확인하고 저장소 루트 global.json에 고정한다. 루트 통합 솔루션과 TM 독립 솔루션에서 같은 SDK 기준을 찾도록 한다. 공통 .NET 스크립트는 저장소 루트를 실행 디렉터리로 지정해 외부 폴더에서 호출해도 같은 SDK를 선택한다. NuGet lock 파일을 최초 생성한 뒤 잠금 restore를 사용한다. SDK/NuGet 다운로드·restore·빌드가 필요한 실행에는 사전 승인을 받는다.

## 6. 펌웨어 VS Code 지원

- Main/Rod 별 C++ 분석 구성을 만들고 현재 VS2022 props와 같은 Core 2.0.17·보드 정의·라이브러리 기준을 사용한다.
- VS Code Build Main/Rod/All은 기존 .cmd를 호출하며 FQBN, Arduino CLI, 코드나 전처리 정의를 별도로 복제하지 않는다.
- 현재 compile_commands의 sketch/staging 경로는 원래 `firmware/...` 경로와 다를 수 있다. 분석용 데이터에서 원본 소스 대응을 생성하고 두 펌웨어를 구분한다.
- compile_commands가 없을 때에는 기존 props에서 분석 정보를 생성하는 기본 설정을 사용한다. 분석 자료 생성 명령이 실제 compile을 호출하는 경우 승인 범위에 포함한다.
- 분석 설정에는 컴파일러의 실제 옵션을 반영한다. 자체 펌웨어 C++98/03 작성 정책을 이유로 Arduino Core의 실제 compiler 표준 옵션을 임의로 바꾸지 않는다.
- MSVC IntelliSense 전용 attribute 우회는 VS2022 분석에만 적용한다. 실제 GCC 빌드나 VS Code 분석에 무조건 주입하지 않는다.
- 작업 폴더를 열 때 빌드·설치·테스트가 자동 시작되는 task를 만들지 않는다. 장비 upload/debug 작업은 추가하지 않는다.

## 7. 구현 순서

세부 요구사항과 완료 근거는 작업 12에 기록한다. 한 번에 한 문자 작업을 수행한다.

| 순서 | 작업 | 주요 산출물 | 완료 조건 |
| --- | --- | --- | --- |
| A | 복구 지점과 계획 | 보존 브랜치, 79개 파일 백업, 계획/작업서 | 생성 완료; 2026-10-02 완료 |
| B | 루트 bin 및 경로 기준 | output/build/clean 관련 스크립트·경로 계약 | 외부 고정 복사 제거, 루트 위치 기준 출력/정리 |
| C | TM 독립 솔루션·도구 기준 | .sln, global.json, NuGet lock | 기존 csproj만 참조, SDK/의존 버전 명시 |
| D | TM Debug/Release 분리 | csproj, profile, Debug 빌드 스크립트 | Debug DLL/PDB와 Release 단일 EXE 설정 분리 |
| E | FW VS Code 설정 | FW workspace, tasks, C++ 분석 자료 생성 | Main/Rod 같은 빌드 스크립트·원본 소스 분석 |
| F | TM VS Code 설정 | TM workspace, tasks, launch 설정 생성 | 공통 Debug 명령과 실제 DLL 경로 연결 |
| G | 인수인계 문서 | 준비/실행/디버깅/경로/문제 해결 안내 | 새 PC 준비 버전과 두 IDE 절차 명시 |
| H | 승인된 빌드·설정 확인 | 실행 로그, 구성/파일/경로 결과 | 승인받은 범위만 수행하고 결과 기록 |
| I | IDE 디버깅·이동 경로 확인 | 사용자 확인 또는 승인된 실행 결과 | 양 IDE 중단점·경로 이동·단일 EXE 실행 확인 |

A/B/D/E/F/G 구현 완료, C는 솔루션/준비 명령 부분 완료. 잔여 문자 3개(C/H/I). 새 설정의 실행 검증은 아직 수행하지 않았다. H/I는 별도의 실행 사전 승인이 필요하다. 과거 01/08/09/10-H/11-I 장비 게이트를 새 작업에 자동 포함하지 않는다.

## 8. 이후 확인할 항목 — 이번에는 미실행

H 단계에서 실행 범위를 제시하고 승인받는다. 예를 들어 Main/Rod Release 각 1회, TM Debug build/Release publish 각 1회, 지정된 구성·파일·PDB·출력 경로 확인을 묶어 제안한다. 반복이나 추가 구성은 새 승인을 받는다.

I 단계에서 다음을 사용자 수동 확인 또는 명시적 승인 범위로 수행한다.

1. TM 전용 솔루션에서 Main/Rod를 빌드하지 않고 TM Debug 실행.
2. VS2022와 VS Code에서 같은 소스의 중단점·단계 실행·지역 변수 확인. Program 진입점 등 장비가 필요 없는 위치를 사용한다.
3. 경로에 공백이 있는 다른 폴더에 복사/clone 후 도구를 준비하고 루트 bin 경로 확인. 도구 설치와 빌드는 승인 범위에 포함한다.
4. 기존 X:가 다른 저장소를 가리킬 때 안전하게 중단하는지 확인. 실제 매핑 변경은 승인한 대상에만 수행한다.
5. Release EXE를 저장소 밖 새 폴더에서 실행해 UI/내장 리소스 로딩 확인. 실제 COM 연결·업데이트·장비 조작은 별도 범위다.
6. FW 결과 4파일과 소스 버전 폴더의 일치, TM 개발/게시 결과가 섞이지 않는지 확인.

IDE 변경 전후 BIN/EXE의 SHA-256이 항상 같다는 것을 호환성의 완료 조건으로 삼지 않는다. 같은 소스·컴파일러·명령·버전·구성·출력 계약의 일치와 필요한 실행 결과를 기준으로 기록한다.

## 9. 변경 경계와 인수인계 조건

- Main/Rod 활성 소스, TM 통신 규약, 장비 동작·타이밍과 UI 기능을 이 작업에서 변경하지 않는다.
- 기존 사용자 변경은 보존한다. legacy/deprecated, 실패 IMU 연구, 과거 장비 미확인 항목을 자동 수정·재개하지 않는다.
- Git에 포함된 설정과 문서만으로 설치가 완료되는 것은 아니다. Arduino CLI/Core/도구, .NET SDK, VS Code 확장을 설치하는 준비 시간이 필요하다.
- 로컬 toolchain은 Git 제외 상태를 유지한다. 고정 버전 준비 절차와 필요한 esptool/리소스 전달 방법을 명시한다.
- commit/push, 플래시, 외부 배포는 이번 범위에 없다.

## 10. 공식 참고 문서

- [VS Code tasks](https://code.visualstudio.com/docs/debugtest/tasks): 공통 명령 호출과 task 설정.
- [VS Code C++ 설정](https://code.visualstudio.com/docs/cpp/customize-cpp-settings): compileCommands와 분석 정보.
- [VS Code C# 디버깅](https://code.visualstudio.com/docs/csharp/debugging), [디버거 설정](https://code.visualstudio.com/docs/csharp/debugger-settings): C# 실행과 coreclr 설정.
- [.NET global.json](https://learn.microsoft.com/en-us/dotnet/core/tools/global-json): 사용할 SDK 버전 선택.
- [C# 심볼 옵션](https://learn.microsoft.com/en-us/dotnet/csharp/language-reference/compiler-options/code-generation): portable PDB 설정.
- [NuGet PackageReference 잠금](https://learn.microsoft.com/en-us/nuget/consume-packages/package-references-in-project-files): packages.lock.json과 잠금 restore.

## 승인 실행 결과

2026-10-02 Main/Rod Release 및 수정 전 TM Debug 성공, TM Release NU1004 실패. 공통 analyzer 설정 수정 후 잠금 갱신/재빌드 대기. 상세: IDE_COMPATIBILITY_RUN_2026-10-02.md.

## 최신 Git 보존 범위 (2026-10-02)

계획 작성 당시 제외했던 commit/push는 이후 사용자의 명시 요청으로 이번 보존 범위에 추가됐다. `backup/local-20261002-143949`는 현재 구현·문서·필수 입력을 보존하는 기준이다. 작업 12 C/H/I와 TM Release NU1004는 미완료로 유지하며 새로운 빌드·테스트 승인을 대신하지 않는다. [백업 기록](BACKUP_LOCAL_2026-10-02.md) 참조.
