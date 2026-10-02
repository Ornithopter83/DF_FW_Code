# VS2022 / VS Code 개발환경 인수인계

Updated: 2026-10-02

## 현재 제공 범위와 미확인 사항

TM 전용 솔루션, FW/TM VS Code workspace, 공통 빌드 명령, Debug/Release 설정과 상대 출력 경로를 작성했다. 승인된 빌드에서 Main/Rod Release와 TM Debug가 성공했다. TM Release는 NU1004 실패 후 잠금 수정 대기이며 실제 실행·중단점·경로 이동 확인은 아직 수행하지 않았다.

**SDK 8.0.419와 초기 잠금은 생성했다.** Release 단일 EXE 의존성이 달라 NU1004가 발생했고 공통 analyzer 설정을 수정했다. 현재는 새 승인 후 잠금 재평가가 필요하다. pin 명령을 다시 실행하지 않는다. 결과: IDE_COMPATIBILITY_RUN_2026-10-02.md.

## 1. 필요한 환경

| 용도 | 기준 |
| --- | --- |
| OS | Windows x64 |
| VS2022 | 17.11 이상, .NET 데스크톱 개발; 기존 FW용 C++ 프로젝트 지원 |
| TM | 안정 .NET 8 SDK, System.IO.Ports 8.0.0 |
| FW | Arduino CLI 1.5.2-rc.1, ESP32 Core 2.0.17, ESP32Servo 1.1.2 |
| VS Code FW | Microsoft C/C++ 확장 ms-vscode.cpptools |
| VS Code TM | Microsoft C# 확장 ms-dotnettools.csharp |

FW 프로젝트는 MSVC로 컴파일하지 않는다. VS2022 NMake와 VS Code 모두 프로젝트 로컬 Arduino CLI 명령을 호출한다. C# Dev Kit 설치는 필수 조건으로 두지 않는다. WinForms 시각적 Designer는 VS2022에서 사용하고 현재 코드 기반 화면 구성은 두 IDE에서 편집한다.

## 2. 최초 준비

Codex가 아래 조회·restore·설치·빌드를 실행하려면 AGENTS.md에 따른 사전 승인이 필요하다. 폴더를 여는 것만으로 실행되지 않는다.

1. 저장소를 clone 또는 전달받는다. 새 exe/이미지 리소스와 ESP32Servo 원본 ZIP 등 필수 입력이 전달됐는지 확인한다. 로컬 toolchain과 복구 백업은 Git 제외이므로 필요하면 따로 전달한다.
2. Windows, VS2022 및 .NET 8 SDK를 준비한다. 저장소 루트에서 한 번 SDK 기준을 기록한다.

```powershell
.\tools\pin-dotnet-sdk.ps1
.\tools\restore-testmodule.cmd
```

- SDK 명령은 설치된 안정 .NET 8 SDK 중 가장 높은 버전을 고르고 루트 global.json에 latestPatch/allowPrerelease=false로 기록한다. 설치된 VS2022가 검색되면 최소 버전도 확인한다.
- global.json이 이미 있으면 자동 교체하지 않는다. 다른 PC는 기록된 SDK의 같은 feature band에서 요구 패치 이상을 설치한다.
- restore 명령은 최초 패키지 잠금 생성/명시적 의존 변경 때만 사용한다. 기존 잠금을 갱신하므로 평상시 빌드 준비 명령으로 반복하지 않는다.
- 일반 빌드 명령은 RestoreLockedMode=true를 사용한다. 패키지 변경이 필요하면 사용자 판단 후 명시적으로 잠금을 갱신한다.
- global.json/packages.lock.json은 Git에 포함하는 공통 입력이다. 초기 잠금은 현재 analyzer 수정 후 재평가가 남은 상태로 `backup/local-20261002-143949` 백업 기준에 보존한다.

3. FW 개발자는 팀이 보관한 같은 버전 toolchain을 루트 toolchain 아래에 준비하거나 PROJECT_COMMANDS의 Core/library 준비 절차를 수행한다. X: 설정은 저장소 루트에서 다음 명령으로 만든다.

```powershell
.\tools\ensure-arduino-drive.ps1
```

X:가 현재 저장소를 가리키면 유지한다. 다른 폴더/물리 드라이브를 가리키면 중단하고 수동 정리를 요구한다. 다른 저장소의 매핑을 자동 해제하지 않는다. CLI config의 data/downloads/user는 기존 X:/toolchain 기준을 사용한다. 출력은 실제 저장소 루트 bin이다.

4. VS Code 설정을 현재 경로와 버전으로 생성한다.

```powershell
.\tools\configure-vscode.ps1 -Target All
```

이 명령은 csproj·VS props·이미 존재하는 compile_commands를 읽고 설정 파일을 생성한다. 컴파일·설치·장비 연결은 하지 않는다. 프로젝트 전용 launch.json/c_cpp_properties.json을 재생성하므로 개인 설정은 별도로 보관한다.

## 3. TM만 개발할 때

### VS2022

- testModule/DFTestModule.sln을 연다.
- Debug / x64, DFTestModule을 시작 프로젝트로 선택한다.
- Program.cs 등 장비가 필요 없는 진입점에 중단점을 놓고 F5 실행한다.
- 일반 Debug는 루트 bin/testmodule/Debug/win-x64의 DLL/PDB를 사용한다.
- 전체 FW 작업이 필요하면 기존 루트 DF_Firmware.sln을 연다. 두 솔루션은 같은 csproj를 사용한다.

### VS Code

- testModule/DFTestModule.code-workspace를 연다.
- 추천 C# 확장을 준비한다.
- Run and Debug에서 TM Debug (shared project)를 선택하고 F5 실행한다.
- preLaunchTask는 공통 Debug 빌드 명령을 호출한다. SDK/잠금이 없으면 안내 후 중단한다.
- 버전을 바꾸면 F5 전에 TM: Configure Debugger task 또는 configure-vscode.ps1 -Target TM을 실행한다. 실행 중 launch 설정을 바꾼 결과는 다음 디버깅에 적용되므로 버전 변경 직후 먼저 재생성한다.
- 장비 연결과 firmware 전송은 사용자 승인한 장비 범위에서만 한다.

### 공통 명령

```powershell
.\tools\build-testmodule-debug.cmd
.\tools\build-testmodule.cmd Release
```

build-testmodule.cmd Debug는 이제 개발용 Debug 빌드로 연결한다. 과거 Debug 단일 EXE 게시/외부 _d 복사와 다르다. 배포 파일은 Release 게시 명령으로 생성한다.

## 4. FW 개발할 때

### VS2022

기존 DF_Firmware.sln과 Main/Rod NMake 프로젝트를 사용한다. TM까지 포함한 통합 Solution Build 대신 FW만 빌드하려면 각 프로젝트 또는 공통 build-all.cmd를 선택한다.

### VS Code

- DF_Firmware.code-workspace를 연다.
- C/C++ 구성에서 DF_Main 또는 DF_Rod를 선택한다.
- FW: Configure IntelliSense를 실행해 현재 toolchain 경로와 원본 소스 대응 분석 자료를 생성한다.
- Build task로 FW: Main Release / Rod Release / All Release를 선택한다.
- 실제 컴파일은 VS2022와 같은 FQBN/CLI/.cmd를 사용한다.

```powershell
.\tools\build-main.cmd Release x64
.\tools\build-rod.cmd Release x64
.\tools\build-all.cmd Release x64
```

compile_commands가 있으면 분석용 복사본을 artifacts/vscode에 만들고 staging src 경로를 원본 firmware 경로에 대응시킨다. 없으면 기존 VS props에서 가져온 include/define 설정을 사용한다. 새 FW 빌드 또는 저장소 이동 후 분석 설정을 재생성한다.

C/C++ 확장의 GCC 분석은 Xtensa MCU를 실제 실행·검증하지 않는다. windows-gcc-x64 모드는 분석용 기본값이며 MCU ABI와 완전한 일치를 보장하지 않는다. 실제 GCC 컴파일 결과와 IDE 화면 확인을 따로 기록한다. 자체 C++03 작성 정책과 Core 컴파일 옵션 gnu++11은 역할이 다르므로 compiler 옵션을 바꾸지 않는다.

ESP32-S3 하드웨어 중단점 디버깅과 upload task는 이번 구성에 없다.

## 5. 출력 위치

| 결과 | 저장소 루트 기준 경로 |
| --- | --- |
| Main | bin/release/x64/<Vm버전>/, 네 flash 파일 |
| Rod | bin/release/x64/<Vr버전>/, 네 flash 파일 |
| FW Debug 경로 | bin/debug/x64/<버전>/; 현재 기존 스크립트의 경로 구분이며 하드웨어 디버그 설정을 뜻하지 않음 |
| TM Debug | bin/testmodule/Debug/win-x64/, DLL/PDB/실행 의존 파일 |
| TM Release | bin/testmodule/Release/DFTestModule_<TmVersion>.exe |
| TM 게시 준비 | artifacts/publish/TestModule/ |

Main/Rod 버전은 기존 Version.cpp를 읽는다. TM 파일명은 csproj의 TmVersion/AssemblyName을 읽는다. 터미널 위치와 SolutionDir에 의존하지 않는다. 고정 DF_OUTPUT 복사를 제거했으며 과거 외부 파일은 삭제하지 않았다.

최종 TM 배포에는 공통 Release 게시 명령을 사용한다. Visual Studio Publish 화면의 FolderProfile도 같은 상대 PublishDir를 사용하지만 공통 스크립트의 임시 게시·정리 절차까지 실행하는 것은 아니다.

## 6. 문제 해결

| 증상 | 조치 |
| --- | --- |
| SDK not pinned | 승인 후 pin-dotnet-sdk.ps1 실행; 생성된 global.json 전달 |
| NuGet lock missing/restore mismatch | 최초 restore 또는 변경한 PackageReference에 대한 잠금 갱신 승인 필요 |
| TM 중단점이 잡히지 않음 | Release EXE 대신 Debug DLL/PDB, 현재 버전 launch 경로 확인; 실제 실행 확인은 아직 대기 |
| X: 다른 폴더 오류 | 다른 작업이 사용 중인지 판단 후 사람이 매핑 정리 |
| 헤더 분석 오류 | 같은 Core/library 준비, configure-vscode 재생성, Main/Rod 구성 선택 |
| 원래 PC의 분석 경로 | 저장소 이동 후 configure-vscode -Target FW 재실행 |
| publish/clean 경로 거부 | 예상 bin/artifacts 대상인지 확인; junction/symlink를 통한 삭제는 거부됨 |

## 7. 복구 및 확인 대기

복구 지점: codex/restore-before-vscode-20261002-114314 및 artifacts/recovery/before-vscode-20261002-114314. 브랜치는 커밋 기준점, ZIP은 당시 미커밋 파일이다. docs/build/RECOVERY_BEFORE_VSCODE_2026-10-02.md를 따른다.

완료: SDK 고정/초기 잠금, 정적 확인, Main/Rod Release와 수정 전 TM Debug. TM Release는 실패, 수정 후 잠금/재빌드는 승인 대기. 미실행: VS2022/VS Code 중단점, 저장소 이동 시험, Release EXE 실행, 복구 재현. 장비 시험은 별도 범위다.

## 8. 현재 백업으로 인수인계

- 보존 태그: `backup/local-20261002-143949`. TM 전용 솔루션과 FW/TM workspace, 필수 esptool/ZIP, 현재 문서를 포함한다.
- [백업 범위와 복원 안내](BACKUP_LOCAL_2026-10-02.md)를 따른다. 새 clone에서도 toolchain 설치와 현재 NuGet 잠금 문제 해결은 별도 준비다.
- `.vscode/arduino.json`은 PC별 포트 설정이므로 Git에서 제외한다. 새 PC에서 포트를 선택한다.
- 기존 작업 전 bundle/ZIP과 빌드 로그는 로컬 artifacts에만 있다. 원격의 백업 태그와 이 ZIP은 서로 다른 시점이다.
- 이 백업은 현재 작업 보존 기준이며 TM Release 성공, 두 IDE 중단점 및 장비 시험 완료를 의미하지 않는다.
