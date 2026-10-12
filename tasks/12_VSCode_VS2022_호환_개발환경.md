# 작업 12

TM 분리 솔루션과 VS Code / VS2022 호환 개발환경

## 목표

같은 소스·빌드 명령·루트 bin 출력을 사용하고, TM을 독립 솔루션 및 VS Code에서 소스 디버깅할 수 있도록 한다.

## 작업 시작 기준 (과거 스냅샷)

- 2026-10-02 `main`, HEAD `0de2d0af61b80dd700b24ae004701b12c49b1f36`.
- 사용자 미커밋 변경 추적 73개/미추적 파일 6개 보존. 스테이징 변경 0개.
- Main/Rod NMake 빌드와 .NET 8 WinForms TM csproj가 존재한다. 루트 솔루션은 세 프로젝트를 참조한다.
- TM Debug 심볼 제거와 게시 profile의 공통 적용, 외부 DF_OUTPUT 고정 복사 보완이 필요하다.
- 2026-10-02 사용자 지시로 판단이 필요한 단계까지 B~G를 일괄 구현했다. C의 SDK 조회·고정/잠금 생성과 H/I 실행은 사전 승인 대기다.

## 관련 영역

- 상세 설계: `docs/build/VSCode_VS2022_COMPATIBILITY_PLAN.md`.
- 복구 기록: `docs/build/RECOVERY_BEFORE_VSCODE_2026-10-02.md`.
- `tools/build-output-env.cmd`, build-main/rod/testmodule 및 해당 clean 스크립트.
- `tools/arduino-env.cmd`, 로컬 Arduino 설정 준비 절차.
- `testModule/DFTestModule.csproj`, 게시 profile, 새 TM 솔루션·workspace·SDK/패키지 기준.
- `vs/DF_Arduino_ESP32S3.props`, 새 FW workspace·분석 설정.
- `PROJECT_COMMANDS.md`, `CurrentWork.md`, `NewThreadHandoff.md`, 인수인계 문서.

## 구현 원칙

- 기존 소스와 프로젝트를 공유하며 IDE별 소스 복사본을 만들지 않는다.
- 설정 값의 기준은 기존 프로젝트/스크립트다. 생성 설정에 버전·보드 정의가 별도로 남지 않게 한다.
- 출력은 실행 디렉터리나 SolutionDir에 의존하지 않고 저장소 루트 bin을 사용한다.
- 기존 C++98/03·고유 접두어·namespace 금지·Variant 분기 금지 정책을 유지한다.
- 빌드/테스트/반복 검증/추가 조사는 AGENTS.md에 따라 명시적 사전 승인을 받는다.

## 세부 작업

### A. 복구 지점과 세부 계획 (완료: 2026-10-02)

- HEAD 보존 브랜치와 bundle, 변경 파일 원본 ZIP, index/patch/manifest 생성.
- 계획과 작업 12를 작성하고 로드맵·현재 상태·인수인계를 연결.
- 확인 근거: 백업 생성 명령 exit 0, 원본 파일 79개 보존, 파일별 SHA-256 기록.
- 복구 재현/빌드/테스트는 미실행. 계획 문서의 파일 저장을 완료 근거로 기록.

### B. 루트 bin 출력과 경로 기준 (구현 완료: 2026-10-02)

- 고정 외부 출력 복사를 기본 스크립트에서 제거한다.
- FW 기존 버전별 4파일 경로와 TM Release 경로 유지; Debug 개발 출력 경로 정의.
- build/clean/script/profile의 경로가 프로젝트 위치에서 계산되도록 정리한다.
- X:가 실제 현재 저장소를 가리키는지 확인하고 충돌 시 중단한다. 기존 매핑을 자동 해제하지 않는다.
- clean/게시 정리는 대상 루트 안으로 제한한다. 기존 외부/과거 출력은 보존한다.
- 완료 조건: 경로 변경 구현과 필요한 확인 목록 기록. 실제 빌드·경로 시험은 H/I에 남긴다.

### C. TM 독립 솔루션과 도구 기준 (완료: 2026-10-02)

- 기존 csproj만 참조하는 `testModule/DFTestModule.sln` 추가. 루트 통합 솔루션 참조 유지.
- 설치된 VS2022와 호환되는 .NET 8 SDK를 선택해 저장소 루트 global.json 기록. 두 솔루션과 .NET 스크립트의 SDK 탐색 시작 경로를 맞춘다.
- System.IO.Ports 버전 유지, packages.lock.json과 잠금 restore 절차 마련.
- SDK 확인·restore 등 필요한 실행 범위를 먼저 승인받는다. 미실행 시 패키지 잠금 확정 완료로 기록하지 않는다.
- 완료 조건: TM-only 프로젝트 참조와 재현 도구 기준 확정.

### D. TM Debug 개발과 Release 게시 분리 (구현 완료: 2026-10-02)

- Debug portable PDB, 심볼, 비최적화, 일반 개발 출력 구성.
- FolderProfile은 게시에 명시 적용하고 Release 단일 EXE 유지.
- 공통 Debug 빌드 스크립트 추가, 기존 publish 스크립트의 TM 버전 중복 제거.
- VS2022와 VS Code가 같은 Debug 결과를 사용하도록 OutputPath/cwd 기준 마련.
- 완료 조건: 구성과 명령 구현. 실제 심볼 생성·중단점 동작은 H/I에서 확인.

### E. FW VS Code workspace와 분석 설정 (구현 완료: 2026-10-02)

- Main/Rod/All tasks에서 기존 .cmd 호출; workspace와 확장 추천 추가.
- Core/보드/라이브러리 정보는 기존 props·compile_commands에서 분석용으로 도출.
- staging 소스를 원본 경로에 대응시키고 Main/Rod 구성을 구분한다.
- MSVC 전용 분석 우회를 GCC에 적용하지 않는다. 폴더 열기 자동 빌드를 만들지 않는다.
- 완료 조건: 설정 생성과 FW 진입점 구현. 실제 분석·빌드는 승인 후 확인.

### F. TM VS Code workspace와 디버거 (구현 완료: 2026-10-02)

- TM 전용 workspace, Debug build task, coreclr launch 설정 구현.
- csproj에서 Debug DLL/버전을 도출해 launch 경로를 생성한다.
- C# 확장 기준과 Debug 준비 명령을 명시한다. 자동 COM 연결/flash task를 추가하지 않는다.
- 완료 조건: 동일 csproj·공통 Debug 명령·생성 경로 연결. 실제 디버깅은 I에서 확인.

### G. 새 PC 준비와 인수인계 문서 (구현 완료: 2026-10-02)

- 고정 버전 Arduino CLI/Core/라이브러리, .NET SDK, VS Code 확장 준비 순서.
- VS2022 TM 전용 솔루션 및 VS Code 두 workspace 열기·분석·빌드·디버깅 절차.
- Debug/Release 결과, 루트 이동, X: 충돌, 실패 코드, 복구 안내 명시.
- 기존 PROJECT_COMMANDS의 오래된 솔루션/출력 설명 갱신; 구현된 내용과 계획 구분.
- 완료 조건: 인수자가 필요한 파일·설치 도구·명령·제약을 문서에서 찾을 수 있음.

### H. 사전 승인된 빌드와 설정 확인

- 실행 대상·구성·횟수·확인 항목을 제시하고 승인 후 수행.
- 제안: Main/Rod Release 각 1회, TM Debug build/Release publish 각 1회, 지정 파일·PDB·경로 확인.
- 실패한 재시도·새 구성·추가 테스트는 승인 범위를 확인하고 필요하면 다시 승인받는다.
- 완료 조건: 승인한 범위의 실제 성공 근거 기록. 미실행 항목을 성공으로 처리하지 않음.

### I. 양 IDE 디버깅과 경로 이동 확인

- 사용자 수동 확인 또는 명시적 승인으로 두 IDE TM 중단점·단계 실행·지역 변수 확인.
- 별도 공백 포함 경로에서 준비/출력 계약 확인; 장비 없는 단일 EXE 실행 확인.
- 실험 대상 X: 매핑 변경과 추가 빌드가 필요하면 해당 범위를 별도 승인받는다.
- 실제 COM 연결·flash·모터 등 장비 동작은 이 작업의 완료 조건에 포함하지 않음.
- 완료 조건: IDE 호환·경로 이동 확인 결과와 남은 제약 기록.

## 진행

잔여 번호 작업 1개 (12)

작업 12 잔여 문자 2개 (H, I)

다음: 수정 후 TM Debug 빌드와 두 IDE/경로 이동 확인의 새 승인 대기. 작업 13의 승인 범위에서 현재 NuGet 잠금 재평가와 TM Release 게시가 성공했다. H의 Debug 및 I는 미완료로 유지한다. 아래 날짜별 실행 결과는 각 시점의 기록이다.

## 변경 금지

- 사용자 기존 미커밋 변경, 활성 펌웨어 동작·프로토콜·타이밍, TM 장비 기능.
- legacy/deprecated 원본, 실패 IMU 연구 및 사용자 제외 장비 게이트.
- 임의 toolchain/Core 업그레이드와 사용자 명시 요청 없는 commit/push/flash/외부 배포. 2026-10-02 문서 최신화·commit/push·백업 태그 지정은 사용자 요청 범위다.

## 완료 조건

- TM 독립 솔루션과 두 VS Code workspace 사용 가능.
- 같은 소스·도구 기준·빌드 명령·출력 계약 사용.
- TM Debug 심볼과 두 IDE 중단점 확인; Release 단일 EXE 유지.
- 루트 bin 상대경로와 저장소 이동 확인.
- 준비 문서·실행 근거·미실행 제약 최신화. 장비 검증 완료로 확대 해석하지 않음.

## 시험 방법

상세 계획의 8절을 따른다. 명령·IDE 실행·재시도는 사전 승인 후 수행하며 이번 A에서는 실행하지 않았다.

## 결과

- 2026-10-02 A 완료: `codex/restore-before-vscode-20261002-114314` 및 `artifacts/recovery/before-vscode-20261002-114314/` 생성.
- HEAD `0de2d0a`, 추적 변경 73개/미추적 파일 6개, 총 79개 원본 파일 보존. 스테이징 변경/삭제 파일 없음.
- 상세 계획, 복구 안내와 작업서 저장. 로드맵·CurrentWork·NewThreadHandoff에 계획 연결.
- 환경 구현, 복구 재현, 빌드·테스트·IDE 검증은 미실행. commit/push/flash/배포 미실행.

## 2026-10-02 일괄 구현 결과

- 사용자 승인 범위: 판단이 필요한 단계까지 일괄 구현. 빌드·테스트·추가 조사의 사전 승인 정책은 유지.
- B 완료: FW/TM 고정 외부 복사 제거, 실제 저장소 기준 bin 출력, X: 일치 확인, 예상 출력 경로와 reparse point를 검사하는 삭제 helper 작성. 정리 명령은 실행하지 않음.
- C 부분 완료: 기존 csproj를 참조하는 TM 전용 .sln과 pin-dotnet-sdk.ps1/restore-testmodule.cmd 작성. SDK/VS2022 조회와 global.json/packages.lock.json 생성은 미실행이며 C 완료로 계산하지 않음.
- D 완료: Debug portable PDB/비최적화/일반 개발 출력, Release profile 게시 분리, csproj에서 버전/AssemblyName 읽기, VS2022 launchSettings의 상대 workingDirectory와 VS Code cwd 기준 일치. Release는 artifacts/publish/TestModule에서 성공 후 bin으로 복사하도록 구현.
- E 완료: FW workspace, Main/Rod 분석 구성, 기존 build-all/Main/Rod 호출 tasks, props/compile_commands 기반 설정 생성 코드 작성.
- F 완료: TM workspace, 공통 Debug build task, coreclr DLL launch, 버전 변경 시 설정 재생성 코드 작성.
- G 완료: docs/build/IDE_HANDOFF.md 및 기존 명령·README·상태 문서 갱신.
- 확인 근거: 각 파일 생성/편집 명령 종료 코드 0. XML/JSON 파일은 입력을 읽어 작성했다. 생성한 PowerShell 스크립트 실행이나 별도 정적 검증을 완료한 것은 아님.
- SDK 조회·restore·compile/link·테스트·IDE 실행·경로 이동·복구 재현·장비 조작은 모두 미실행.
- 잔여 문자 3개(C/H/I). 현재는 실행 승인과 이후 사용자 IDE 확인이 필요하다.

## 2026-10-02 승인된 준비·빌드 실행 결과

- 사용자 승인: SDK/VS2022 조회·명시적 NuGet restore 1회, 정적 확인 1회, Main/Rod Release 각각 1회, TM Debug/Release 각각 1회.
- 정적 확인: PowerShell 7개/JSON 11개/XML 2개 오류 없음. 공통 analyzer 수정 이후 재확인은 하지 않음.
- SDK 8.0.419와 VS2022 17.14.37027.9 확인, global.json 및 초기 packages.lock.json 생성 완료.
- Main Release exit 0: 888,768-byte BIN, Vm1.0.10.0 폴더 네 파일.
- Rod Release exit 0: 766,848-byte BIN, Vr1.0.2.0 폴더 네 파일.
- TM Debug exit 0: 경고/오류 0, DLL 11,361,280 bytes 및 portable PDB 47,680 bytes/BSJB 생성.
- TM Release exit 1: 단일 EXE 설정에서 Microsoft.NET.ILLink.Tasks 8.0.25 추가로 NU1004 잠금 불일치.
- 공통 EnableSingleFileAnalyzer=true 수정은 작성했으나 잠금 갱신/재빌드는 미실행. Debug 성공 산출물은 이 수정 전 결과다.
- C는 SDK 고정까지 완료했으나 수정 후 잠금 갱신이 남아 부분 완료로 유지. H는 Release 실패로 미완료. I는 IDE/이동 확인 미실행.
- 재시도 0회, 잔여 문자 3개(C/H/I). 다음 NuGet 잠금 재평가 1회 + TM Debug/Release 각 1회는 새 승인 대기.
- 상세: docs/build/IDE_COMPATIBILITY_RUN_2026-10-02.md, 로그/파일 hash: artifacts/ide-compat-20261002/.

## 2026-10-02 문서 최신화 및 백업 지정

- 사용자 명시 요청: 문서 최신화, 로컬 버전 commit/push, 백업본 지정.
- 백업 기준 태그: `backup/local-20261002-143949`. 원격 main과 같은 커밋을 지정한다.
- 기존 작업 전 복구 지점과 최신 Git 스냅샷의 차이, 백업 포함/제외 범위, TM NU1004 및 미확인 항목을 현재 상태·인수인계·명령·복구 문서에 반영한다.
- PC별 포트 설정은 로컬 유지, 필수 esptool/ESP32Servo ZIP·학습 문서와 공통 개발 설정은 커밋에 포함한다.
- 이번 Git 보존 작업은 C/H/I 완료 조건을 대신하지 않는다. 추가 빌드·테스트·IDE/장비 확인은 미실행으로 유지한다.
- 잔여 번호 작업 1개(12), 잔여 문자 3개(C/H/I).
- Git 커밋 및 원격 반영 근거는 해당 태그의 커밋과 푸시 결과다. 상세 보존 범위는 `docs/build/BACKUP_LOCAL_2026-10-02.md`에 기록한다.

## 2026-10-02 작업 13의 승인 빌드 후 갱신

- 사용자 코드 조사·표시 수정·빌드 승인에 따라 명시 restore 1회와 TM Release 게시 1회 실행, 모두 exit 0.
- 12-C 완료 근거: SDK/독립 솔루션 기준은 이미 작성됐고, 공통 analyzer 수정 후 현재 잠금 재평가와 Release locked restore·단일 EXE 게시가 성공했다. packages.lock.json의 Git 내용 차이는 없다.
- 12-H 부분 진행: Release 성공. Main/Rod는 표시 수정의 영향 범위 밖이며 이전 성공 기록을 보존한다. 수정 후 TM Debug 빌드는 미실행으로 남긴다.
- 12-I: 두 IDE 중단점/단계 실행·저장소 이동 미실행.
- 잔여 번호 1개(12), 잔여 문자 2개(H/I). 상세: docs/build/TM_DISPLAY_CORRECTION_2026-10-02.md.
