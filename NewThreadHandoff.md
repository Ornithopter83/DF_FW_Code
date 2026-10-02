# DF Firmware 스레드 인수인계

Updated: 2026-10-02

## 최우선 정책

- 이주/인수인계만 요청받으면 AGENTS.md와 이 문서만 읽고 짧게 보고한 뒤 기다린다. Git·소스 탐색·전체 문서 재독·작업 자동 재개를 하지 않는다.
- 빌드(compile/link), 테스트, 반복 검증, 추가 조사·탐색은 사용자 명시적 사전 승인 후 수행한다. 코드 수정 지시나 과거 승인은 포괄 승인이 아니다. 재시도와 범위 확장도 승인 범위를 확인한다.
- 명시적 요청 없이 commit/push/flash/배포/외부 시스템 변경을 수행하지 않는다. 플래시와 장비 시험은 사용자가 수동 수행한다.

## 현재 기준

- 저장소: `C:\Projects\AI-AGENTS\DF_FW_Code`.
- 연동 문서: `C:\Projects\AI-AGENTS\DF_Integration`.
- 현재 체크아웃은 `main`. 보존 기준은 주석 태그 `backup/local-20261002-143949`이며 이번 문서와 로컬 변경을 포함하는 커밋을 지정한다.
- 백업 전 HEAD는 `0de2d0a`, 원격 main은 `c84be58`(2026-09-11)이었다. 원격 refs 조회에서 main만 존재했고 legacy 원본을 확인했다. 사용자 요청으로 commit/push와 백업 태그 지정 범위를 승인받았다. 자동 pull/merge/reset은 수행하지 않는다.
- 최신 백업 범위와 복원 안내: `docs/build/BACKUP_LOCAL_2026-10-02.md`. 과거 원격 비교: `docs/build/REMOTE_BACKUP_COMPARISON_2026-10-02.md`.
- Main/Rod 독립 펌웨어 + 공통 DFProtocol + .NET 8 Windows x64 WinForms TM. 활성 소스는 firmware/DF_Main, firmware/DF_Rod, libraries/DFProtocol, testModule이다.
- 현재 루트 DF_Firmware.sln에 세 프로젝트가 있으며 같은 csproj를 참조하는 TM 전용 .sln과 FW/TM VS Code workspace를 추가했다.
- 현재 소스 버전 기록은 Main Vm1.0.10.0 / Rod Vr1.0.2.0 / TM V041. 과거 성공 산출물을 현재 변경의 검증 결과로 사용하지 않는다.

## 마지막 완료 작업과 복구

- 사용자 요청: 복구 지점과 계획 작성, 개발환경 일괄 구현, 승인된 준비·빌드 실행을 거쳐 문서 최신화 및 로컬 버전 원격 백업 지정. 백업 지정은 작업 12의 빌드·IDE 확인 완료를 의미하지 않는다.
- 작업 12-A/B/D/E/F/G 구현 완료. 승인된 정적 확인 및 Main/Rod/TM Debug 빌드는 성공. C는 수정 후 잠금 갱신이 남고 H는 Release 실패로 미완료. I/복구 재현/장비 시험은 미실행.
- 보존 브랜치: `codex/restore-before-vscode-20261002-114314`, 커밋 기준점만 보존.
- 미커밋 백업: `artifacts/recovery/before-vscode-20261002-114314/`. 추적 변경 73개 + 미추적 파일 6개, 원본 79개 보존; 스테이징 변경/삭제 파일 없음.
- bundle, 원본 ZIP, index, patch, 파일별 SHA-256 manifest 생성 명령 exit 0. Git-ignored 로컬 toolchain/추가 산출물/개인 설정 및 외부 출력은 제외했다.
- 복원은 새 폴더에서 수행한다. 백업은 Git 제외 로컬 파일이므로 전달 시 별도 복사해야 한다.
- 복구 안내: `docs/build/RECOVERY_BEFORE_VSCODE_2026-10-02.md`.

## 계획과 다음 작업

- 목표: TM 분리 솔루션, VS2022/VS Code 같은 소스·빌드 명령·TM 디버깅, 루트 bin 상대경로 출력.
- 작업서: `tasks/12_VSCode_VS2022_호환_개발환경.md`.
- 상세 계획: `docs/build/VSCode_VS2022_COMPATIBILITY_PLAN.md`. 사용 절차: `docs/build/IDE_HANDOFF.md`.
- 잔여 번호 작업 1개(12), 작업 12 잔여 문자 3개(C/H/I).
- SDK 8.0.419 / VS2022 17.14.37027.9 확인, global.json과 초기 잠금 생성. Main/Rod Release와 TM Debug 성공, TM Release NU1004 실패. 공통 EnableSingleFileAnalyzer=true 수정 후 잠금 갱신/재빌드는 미실행. 다음 NuGet 잠금 재평가 1회 + TM Debug/Release 각 1회는 새 승인 필요. I는 미실행.
- 순서: B 경로 → C TM 솔루션/SDK → D Debug/Release → E FW VS Code → F TM VS Code → G 문서 → H 승인 빌드 → I IDE/이동 확인.
- TM Debug portable PDB/비최적화 설정과 Release 단일 EXE 게시를 분리했다. 고정 외부 복사를 제거하고 X: 일치/정리 범위 확인 코드를 작성했다. Main/Rod 출력 및 수정 전 TM Debug DLL/PDB는 확인했다. TM Release는 실패했고 공통 analyzer 수정 후 확인은 미실행이다. FW 하드웨어 중단점 디버깅은 별도 범위다.
- 후속 실행 지시를 받으면 로드맵 → CurrentWork → PROJECT_COMMANDS → 작업 12 순서로 읽는다. H/I의 실행은 별도 사전 승인이 필요하다.

## 보존할 기존 경계

- 사용자 미커밋 변경과 legacy/deprecated를 보존한다. 자체 FW 코드는 C++98/03, 고유 접두어, namespace/새 Variant 분기 금지.
- 기존 11-J 무선 업데이트 보완, 11-K LM JIG 구현은 완료 기록 유지. 물리 장비 검증 완료를 의미하지 않는다.
- 01 잔여/08-JK/09 전체/10-H/11-I는 사용자 제외 범위다. 새 IDE 계획에 자동 포함하지 않는다.
- 실패 IMU 보정 연구와 BLDC 추가 분석을 자동 재개하지 않는다. 실패 연구 보관 브랜치는 기존 기록으로 유지하며 자동 병합·삭제하지 않는다.
- TM의 통신/업데이트/장비 기능과 Main/Rod 동작·타이밍은 개발환경 정리에서 변경하지 않는다.

## 이번 실행 근거

상세 결과: docs/build/IDE_COMPATIBILITY_RUN_2026-10-02.md. 로그/hash: artifacts/ide-compat-20261002/. 승인된 네 빌드를 각각 한 번 실행했고 재시도하지 않았다. SDK/잠금 수정 후 빌드는 별도 승인이 필요하다.

## 2026-10-02 백업 기준

- 태그: `backup/local-20261002-143949`. 동일 커밋을 origin/main과 백업 태그에 보존하는 기준이다.
- 소스·공통 IDE 설정·SDK/초기 NuGet 잠금·문서·필수 ZIP/esptool과 기존 추적 산출물 변경을 포함한다.
- toolchain, 루트 bin, artifacts, 개인 설정 및 `.vscode/arduino.json`은 원격 백업에 포함하지 않는다. 기존 79파일 작업 전 ZIP은 로컬 보존을 유지한다.
- 이 문서 작성 및 Git 보존 과정에서 추가 빌드·테스트·장비 조작은 실행하지 않는다. C/H/I 잔여 3개는 유지한다.
