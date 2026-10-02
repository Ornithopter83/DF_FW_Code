# 로컬 버전 백업 기준 — 2026-10-02

## 보존 참조

| 항목 | 기준 |
| --- | --- |
| 저장소 | https://github.com/Ornithopter83/DF_FW_Code |
| 작업 브랜치 / 원격 대상 | main / origin/main |
| 주석 백업 태그 | `backup/local-20261002-143949` |
| 태그 대상 | 이 문서와 문서 최신화·로컬 변경을 포함하는 동일 커밋 |
| 커밋 전 로컬 HEAD | 0de2d0af61b80dd700b24ae004701b12c49b1f36 |
| 푸시 전 원격 main | c84be5845e7ce1edd616968e18f78c8d8002c089 (2026-09-11) |

사용자가 문서 최신화, 로컬 버전 커밋·푸시 및 백업 지정을 명시적으로 요청했다. main은 앞으로 변경될 수 있으므로 이 시점의 보존 기준은 태그를 사용한다. 태그를 이동하거나 덮어쓰지 않는다. 실제 커밋 SHA는 이 태그를 해석한 결과다.

## 포함한 범위

- 현재 Main/Rod·DFProtocol·TM 소스 및 기존 사용자 변경.
- TM 독립 솔루션, FW/TM VS Code workspace와 공통 IDE/빌드/디버깅 설정.
- global.json, 현재 초기 packages.lock.json 및 경로·복원·빌드 helper.
- 로드맵, 현재 작업, 인수인계, 작업 12, 비교·개발환경 문서와 학습 문서.
- 필수 testModule/esptool_V4.5.1.exe 및 ESP32Servo-1.1.2.zip.
- 이미 Git에서 추적 중인 산출물·캐시·로그의 현재 변경. 보존용이며 새 빌드 성공의 근거는 아니다.
- 커밋 전 로컬에만 있던 9월 15일 커밋도 main의 이력에 포함한다.

## 로컬에만 남는 범위

- Git 제외 toolchain, 루트 bin의 펌웨어/TM 출력, artifacts의 빌드 근거 및 기존 복구 bundle/ZIP.
- 개인 환경 설정, 인증 정보와 PC별 `.vscode/arduino.json` 포트 설정.
- 따라서 이 태그는 개발 PC 전체 이미지나 최신 배포 바이너리 묶음이 아니다. 새 PC에서는 문서에 따른 도구 준비가 필요하다.

## 구현 및 확인 상태

- Main Vm1.0.10.0 / Rod Vr1.0.2.0 / TM V041.
- 작업 12-A/B/D/E/F/G 구현 완료. SDK 8.0.419와 초기 잠금 생성.
- 앞선 승인 실행에서 Main/Rod Release 및 수정 전 TM Debug 성공.
- TM Release는 NU1004 실패. 공통 EnableSingleFileAnalyzer=true 수정 후 잠금 재평가와 재빌드는 미실행이다.
- 두 IDE 중단점·단계 실행, 저장소 이동 및 장비 확인은 미실행.
- 잔여 번호 1개(12), 잔여 문자 3개(C/H/I). 이번 백업 지정은 검증된 릴리스나 해당 잔여 작업의 완료를 의미하지 않는다.
- 이번 문서 최신화·Git 보존에서 추가 빌드·테스트·플래시는 실행하지 않는다.

## 다른 복구 지점과의 차이

`codex/restore-before-vscode-20261002-114314`와 로컬 `artifacts/recovery/before-vscode-20261002-114314/`는 작업 시작 전 기준이다. 이 최신 태그는 그 이후 개발환경 구현과 문서를 포함한다. 작업 전 백업은 로컬 보존을 유지하며 이번 원격 태그에 ZIP을 포함하지 않는다.

## 새 폴더 복원 안내 — 실행하지 않은 명령

PowerShell에서 아직 존재하지 않는 대상 폴더명을 정한 뒤 실행한다. 기존 작업 폴더에서 reset/clean을 실행하는 복원 방식은 사용하지 않는다.

```powershell
$dfRestore = '.\DF_FW_Code_backup_20261002'
if (Test-Path -LiteralPath $dfRestore) { throw '새 복원 폴더를 지정하세요.' }
git clone --branch 'backup/local-20261002-143949' --single-branch https://github.com/Ornithopter83/DF_FW_Code.git $dfRestore
if ($LASTEXITCODE -ne 0) { throw '백업 clone 실패' }
```

태그 clone은 detached HEAD로 열린다. 그 폴더에서 수정 작업을 시작할 때는 별도 작업 브랜치를 만든다. 도구 설치·잠금 갱신·빌드·시험 및 Codex의 추가 조사는 AGENTS.md의 사전 승인 정책을 따른다.

## 관련 문서

- [현재 작업](../../CurrentWork.md)
- [인수인계](../../NewThreadHandoff.md)
- [개발환경 사용 안내](IDE_HANDOFF.md)
- [백업 전 원격 비교](REMOTE_BACKUP_COMPARISON_2026-10-02.md)
- [작업 전 복구 지점](RECOVERY_BEFORE_VSCODE_2026-10-02.md)
- [기존 승인 실행 결과](IDE_COMPATIBILITY_RUN_2026-10-02.md)
