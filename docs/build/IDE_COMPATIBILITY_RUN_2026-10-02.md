# 호환 개발환경 승인 실행 결과

작성일: 2026-10-02

## 승인과 실행 횟수

사용자가 SDK/VS2022 조회·NuGet 복원 1회, 설정 정적 확인 1회, Main/Rod Release 각 1회, TM Debug/Release 각 1회를 승인했다. 재시도는 수행하지 않았다.

| 항목 | 실행 횟수 | 결과 |
| --- | --- | --- |
| 설정 정적 확인 | 1회 | PS 7개/JSON 11개/XML 2개 파싱 오류 없음 |
| SDK/VS2022 조회·SDK 고정 | 1회 | .NET 8.0.419 / VS2022 17.14.37027.9, 루트 global.json 생성 |
| 최초 NuGet 명시적 복원 | 1회 | 성공, packages.lock.json 생성 |
| Main Release x64 | 1회 | 성공, Vm1.0.10.0 폴더 4파일 |
| Rod Release x64 | 1회 | 성공, Vr1.0.2.0 폴더 4파일 |
| TM Debug | 1회 | 성공, 경고/오류 0, DLL과 portable PDB 생성 |
| TM Release 단일 EXE 게시 | 1회 | 실패, NU1004 잠금 불일치 |

TM 빌드 내부에서 잠긴 모드의 restore가 실행됐다. 최초 잠금 재평가는 위 명시적 복원 한 번뿐이다.

## 실제 산출물

- Main: bin/release/x64/Vm1.0.10.0/DF_Main.ino.bin, 888,768 bytes.
- Rod: bin/release/x64/Vr1.0.2.0/DF_Rod.ino.bin, 766,848 bytes.
- 각 FW 폴더는 application/bootloader/partitions/boot_app0 네 파일.
- TM Debug: bin/testmodule/Debug/win-x64/DFTestModule_V041.dll, 11,361,280 bytes.
- TM Debug PDB: 같은 폴더의 DFTestModule_V041.pdb, 47,680 bytes, portable PDB 서명 BSJB 확인.
- 이번 Release 게시 EXE는 생성되지 않았다. 기존 bin/testmodule/Release의 파일을 이번 성공 결과로 사용하지 않는다. 실패 전 기존 Release 폴더는 교체하지 않았다.
- 파일별 SHA-256과 종료 코드: artifacts/ide-compat-20261002/approved-run-results.json.

## 실패와 작성한 수정

Release의 PublishSingleFile 설정에서 SDK가 Microsoft.NET.ILLink.Tasks 8.0.25 의존성을 추가했다. 최초 일반 restore의 잠금에는 System.IO.Ports만 있어 NU1004로 중단됐다.

공통 csproj에 EnableSingleFileAnalyzer=true를 추가해 일반 Debug와 단일 EXE 게시의 restore에 ILLink 의존성을 함께 포함하도록 수정했다. 수정 후 잠금 갱신·정적 재확인·빌드 재시도는 실행하지 않았다. 수정이 해결됐다는 검증 결과는 아직 없다.

현재 SDK 파일과 초기 잠금은 존재하지만, 수정한 프로젝트 기준 잠금 갱신이 필요하다. 다음 승인 범위 제안은 NuGet 잠금 재평가 1회와 TM Debug/Release 각 1회다. Main/Rod 재빌드는 요구하지 않는다.

## 확인되지 않은 항목

VS2022/VS Code 실제 중단점·단계 실행, FW 설정 생성 명령의 전체 실행, 저장소 이동/공백 경로, Release EXE 실행, 복구 재현, 장비 동작은 미실행이다. 플래시·장비 명령·commit/push/외부 배포는 수행하지 않았다.

## 로그

artifacts/ide-compat-20261002 아래 static-check.json, sdk-pin.log, nuget-restore.log, main-release.log, rod-release.log, tm-debug.log, tm-release-publish.log, approved-run-results.json.
