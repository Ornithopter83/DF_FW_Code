# VS Code / VS2022 환경 정리 전 복구 지점

작성일: 2026-10-02

## 보존한 상태

| 항목 | 실제 결과 |
| --- | --- |
| 저장소 | `C:\Projects\AI-AGENTS\DF_FW_Code` |
| 작업 브랜치 | `main` 유지 |
| 기준 커밋 | `0de2d0af61b80dd700b24ae004701b12c49b1f36` |
| 로컬 보존 브랜치 | `codex/restore-before-vscode-20261002-114314` |
| 추적 파일 변경 | 73개 |
| 미추적 파일 | 6개, 디렉터리 안 파일까지 개별 보존 |
| 스테이징 변경 | 0개 |
| 원본 파일 백업 | 총 79개, 줄바꿈과 바이너리 원본 바이트 보존 |
| Git 상태 | 당시 `main...origin/main [ahead 1]`; 원격 조회/fetch 미실행 |

보존 브랜치는 **커밋된 기준점**을 가리킨다. 미커밋 변경은 아래 ZIP에 별도로 들어 있다. 브랜치만 전환해서는 당시 미커밋 상태가 복구되지 않는다.

## 백업 위치와 파일

`artifacts/recovery/before-vscode-20261002-114314/`

| 파일 | 용도 |
| --- | --- |
| `committed-history.bundle` | 보존 브랜치와 그 커밋 이력; 91,603,957 bytes |
| `working-files.zip` | 변경된 추적 파일과 무시되지 않은 미추적 파일; 28,791,255 bytes |
| `manifest.json` | 파일 목록·상태·원본 파일별 SHA-256·제외 범위 |
| `status-before.bin` | Git porcelain 상태 원본, NUL 구분 |
| `staged.patch` | 당시 스테이징 변경; 이번에는 없음 |
| `unstaged.patch` | 미커밋 변경의 보조 diff; ZIP의 원본 바이트가 복구 기준 |
| `index-before` | 원래 Git index 보관본 |
| `RESTORE.md` | 백업과 함께 보관하는 복구 안내 |

`artifacts/`는 Git 제외 경로이므로 백업 파일은 원격으로 전달되지 않는다. 새 PC에 복구 지점까지 넘길 때는 이 디렉터리를 따로 전달한다.

## 복구 범위

- 변경된 소스·설정·문서, 추적 중인 bin/obj 바이너리도 보존했다.
- 미추적 `ESP32Servo-1.1.2.zip`, `testModule/esptool_V4.5.1.exe`, `docs/learning/` 안 파일도 포함했다.
- Git에서 무시되며 추적되지 않는 `toolchain/`, 추가 산출물, 개인 로컬 설정과 외부 출력 폴더는 포함하지 않았다. 이 백업은 설치 환경 전체의 이미지가 아니다.
- 이번에 추가한 계획 문서는 복구 지점 생성 이후 파일이므로 이 백업에 포함되지 않는다.

## 복구 절차 — 실행하지 않은 안내

원래 작업 폴더를 유지하고 새 폴더에 복원한다. Git과 PowerShell이 준비된 환경에서 다음 예시를 사용한다. 새 대상 폴더가 이미 있으면 다른 빈 경로를 정한다.

```powershell
$dfBackup = 'C:\Projects\AI-AGENTS\DF_FW_Code\artifacts\recovery\before-vscode-20261002-114314'
$dfRestore = 'C:\Projects\AI-AGENTS\DF_FW_Code_restore_20261002'
if (Test-Path -LiteralPath $dfRestore) { throw '복구 대상은 새 폴더여야 합니다.' }
git clone --branch codex/restore-before-vscode-20261002-114314 "$dfBackup\committed-history.bundle" $dfRestore
if ($LASTEXITCODE -ne 0) { throw '기준 커밋 복원 실패' }
Expand-Archive -LiteralPath "$dfBackup\working-files.zip" -DestinationPath $dfRestore -Force
```

이번 복구 지점에는 스테이징 변경과 삭제 파일이 없어 추가 적용·삭제가 필요 없다. 다른 복구 지점에 이 절차를 재사용할 때는 manifest의 스테이징·삭제 목록을 먼저 확인한다. ZIP 적용 후 `unstaged.patch`를 다시 적용하지 않는다. `index-before`를 다른 clone의 index에 바로 덮어쓰지 않는다.

복구 확인이 승인되면 새 폴더에서 HEAD, 파일별 SHA-256, Git 변경 목록을 대조한다. toolchain 재설치와 빌드는 별도 승인 범위다. 원래 폴더에서 reset/clean을 수행하는 절차는 사용하지 않는다.

## 수행 근거와 미실행 항목

- 백업 생성 명령 종료 코드 0. 보존 브랜치, bundle, ZIP, index, patch, manifest 생성 완료.
- 파일별 SHA-256은 ZIP에 넣은 원본 바이트로 기록했다.
- 복구 재현 시험, 빌드, 테스트, IDE 디버깅, 플래시, 배포는 미실행.
- 기존 작업 트리와 스테이징을 수정하거나 commit/push하지 않았다.

## 이후 생성한 최신 Git 백업과의 구분

위 복구 지점은 개발환경 변경 전 상태다. 이후 구현·문서를 포함하는 최신 보존 기준은 `backup/local-20261002-143949`이며 [최신 백업 안내](BACKUP_LOCAL_2026-10-02.md)를 따른다. 원격 태그에는 이번 커밋에 포함한 파일이 들어 있고, 이 문서의 기존 bundle/ZIP과 Git 제외 환경은 자동 업로드되지 않는다. 과거 복구 지점은 삭제하거나 덮어쓰지 않는다.
