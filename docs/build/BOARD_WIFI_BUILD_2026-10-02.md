# Board Wi-Fi 빌드 결과 — 2026-10-02

## 최신 결과 — 자체 포함 단일 EXE 게시 성공

사용자는 Connector 재시도 1회와 자체 포함 단일 파일 게시를 명시적으로 요청했다. 중단된 직전 턴에서는 실행 명령을 호출하지 않았으며, 후속 턴에서 실제 게시 1회만 실행했다. exit 0.

- 파일: bin/BoardWifiConnector/Release/BoardWifiConnector.exe.
- 크기: 161,859,788 bytes.
- 옵션: win-x64, SelfContained=true, PublishSingleFile=true, IncludeNativeLibrariesForSelfExtract=true.
- 네이티브 런타임을 EXE에 포함하며 런타임이 실행 시 필요한 파일을 자체 추출한다. 대상 PC에 .NET 설치가 필요 없는 구성이다.
- 기본 INI 양식을 EXE 리소스로 포함했다. Release PDB를 제외하고 Release 중간 산출물을 artifacts로 이동했다. 최초 실행 시 EXE 옆에 connection.ini가 생성되며 이미 존재하는 설정은 보존한다.
- 이전 CS8510 예외 패턴 오류는 이번 게시에서 발생하지 않았다.
- 명시적인 NuGet 잠금 재평가/갱신은 추가 실행하지 않았다. 게시 내부의 잠금 복원은 모든 프로젝트가 최신 상태라고 보고했다.
- 근거: artifacts/boardwifi-build-20261002-02/connector-singlefile.log, connector-singlefile.exit.txt, result.json.

16-C 완료: 2026-10-02. Main 빌드 및 Connector 게시가 성공했으며 잔여 번호 2개(12,15), 잔여 문자 3개(12-H/I,15-B). EXE 실행·정적/GUI/통신/장비 테스트·설치·최초 등록·자동 시작 등록·실제 Windows 종료·플래시·commit/push는 미실행이다. 아래 실패 결과는 첫 게시 시점의 이력이다.

## 승인 및 실제 실행

사용자는 Main 빌드 1회 + BoardWifiConnector NuGet 복원 1회 + Release 게시 1회의 제안에 진행을 승인했다. 각 실행은 한 번만 수행했으며 실패 재시도는 하지 않았다. Release 게시 내부의 잠금 기준 복원은 게시 스크립트에 포함된다.

| 실행 | 결과 | 근거 |
| --- | --- | --- |
| tools/build-main.cmd Release x64 | 성공, exit 0 | artifacts/boardwifi-build-20261002-01/main.log 및 main.exit.txt |
| BoardWifiConnector/build.ps1 -Mode Restore | 성공, exit 0 | connector-restore.log 및 connector-restore.exit.txt |
| BoardWifiConnector/build.ps1 -Mode Publish | 실패, exit 1 | connector-release.log 및 connector-release.exit.txt |

## Main 산출물

빌드가 보고한 출력 경로: bin/release/x64/Vm1.0.10.0.

| 파일 | bytes |
| --- | ---: |
| DF_Main.ino.bin | 909,696 |
| DF_Main.ino.bootloader.bin | 14,032 |
| DF_Main.ino.partitions.bin | 3,072 |
| boot_app0.bin | 8,192 |

Arduino 보고: Sketch 909,337 bytes / 1,310,720, global variables 155,772 bytes / 327,680. 출력 파일 목록과 크기는 빌드 로그의 보고값이며 별도의 재검증이나 해시 검사는 하지 않았다. 기존 같은 버전의 Main 산출물을 새 AP/TCP 포함 빌드로 갱신했다. 플래시/장비 시험은 하지 않았다.

## Connector 실패와 소스 수정

NuGet 명시적 복원은 성공하여 packages.lock.json을 생성했다. Release 게시 중 BoardConnection.cs의 FriendlyError switch에서 CS8510이 발생했다. SocketException은 Win32Exception을 상속하므로 일반 Win32Exception 패턴 뒤의 SocketException 패턴에 도달할 수 없었다.

SocketException 패턴을 Win32Exception 앞에 배치하는 소스 수정을 했다. 수정 후 빌드·정적 검사·테스트는 미실행이다. 새 BoardWifiConnector.exe는 생성되지 않았으며 수정 성공은 다음 승인된 빌드에서 확인해야 한다.

## 다음 실행 및 제외 범위

Connector Release 게시 재시도 1회는 AGENTS.md에 따른 새 사용자 사전 승인이 필요하다. 잠금 갱신이나 Main 재빌드는 다음 재시도 범위에 포함하지 않는다.

Connector Debug, 자체/GUI/통신 테스트, 설치·자동 시작 등록·실제 Windows 종료, Main/Rod 플래시 및 장비 시험, TM 빌드, commit/push 모두 미실행이다. 잔여 번호 3개(12,15,16), 잔여 문자 4개(12-H/I,15-B,16-C).
