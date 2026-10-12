# BoardWifiConnector

Main의 전용 Wi-Fi AP에 연결하여 게임이 실행되지 않을 때 전원버튼 종료 요청을 받는 Windows 트레이 프로그램이다. .NET 8 WinForms, Windows x64이며 별도 게임/TM 시리얼 연결을 상시 점유하지 않는다.

## 현재 상태 (2026-10-02)

최신: 승인된 재시도 1회에서 자체 포함 단일 EXE 게시 성공(exit 0). bin/BoardWifiConnector/Release/BoardWifiConnector.exe, 161,859,788 bytes. .NET 런타임과 네이티브 라이브러리, 기본 INI 양식을 EXE에 포함했다. 별도 .NET 설치 없이 실행하는 구성이며 최초 실행 시 connection.ini를 옆에 생성한다. 설정 파일과 등록 후 생성되는 credential.dat는 사용자가 변경/보관할 외부 파일이다. 실제 실행/통신/장비/Windows 종료 시험은 미실행이다. 아래 실패·EXE 없음 설명은 이전 게시 시점의 이력이다.

최신 빌드: 승인된 Main 빌드와 Connector 명시 복원은 성공했다. Connector Release 게시 1회는 CS8510 예외 패턴 순서 오류로 실패했다. 순서를 소스에서 수정했으나 수정 후 재게시/테스트는 미실행이다. packages.lock.json은 생성됐고 배포 EXE는 아직 없다. 다음 게시 1회는 새 사전 승인 필요. 상세: ../docs/build/BOARD_WIFI_BUILD_2026-10-02.md. 아래 최초 구현 시점의 미실행 설명은 당시 기록이다.

소스 구현 완료. NuGet 복원, 잠금 파일 생성, 빌드, 정적/실행 테스트, 장비 시험, 설치, 자동 시작 등록, 실제 Windows 종료는 미실행이다. 배포 EXE는 아직 없다. Codex의 실행에는 AGENTS.md에 따른 새 사전 승인이 필요하다.

사용자는 로그온 후 트레이 동작이면 충분하다고 확인했다. 로그인 전 처리를 위한 Windows 서비스는 구현 범위에 포함하지 않는다.

## 통신 구성

```text
PC 로그온 → 트레이 시작 → 지정 Windows Wi-Fi 프로필 연결
                         → 보드의 고정 AP 주소/TCP 포트 연결
메인보드 전원버튼
  ├─ 게임 시리얼 연결 유효 → 기존 게임 종료 요청/응답
  └─ 게임 연결 없음       → TCP 종료 요청 → Connector → Windows 정상 종료 요청
Windows 요청 접수 → TCP ACK → 기존 보드 USB5V 대기 → 기존 전원 차단
```

- 외부 공유기·인터넷 불필요. PC에는 2.4GHz Wi-Fi 어댑터가 필요하다.
- 보드는 AP+STA 모드, AP와 기존 Main/Rod ESP-NOW 모두 채널 1을 사용한다. ESP-NOW peer의 STA 인터페이스를 유지하며 외부 AP에 연결하지 않는다.
- 고정 AP 주소는 일반적인 SoftAP 기본 주소, TCP 포트는 45180이다. AP SSID는 보드에서 생성한 식별값을 붙여 기기별로 구분한다.
- 게임 연결은 기존 CheckAppCmd의 최근 4초 시리얼 수신/3회 이상 명령 기준과 AP_IS_DF 타입, 시리얼 활성 상태를 함께 사용한다. 정확한 게임 생존 신호를 새로 추가한 구현은 아니다. TM 연결은 게임 경로로 선택하지 않는다.
- 종료 요청을 시리얼로 시작했더라도 게임 연결이 끊기면 Wi-Fi로 전환한다. Wi-Fi로 전환한 요청은 게임 연결이 복구돼도 중복 전송하지 않는다.
- 기존 시리얼 $18 응답 및 USB5V 하강 후 차단 흐름을 보존한다. 기존 최대 2분 강제 차단 조건도 유지한다. 수신만 하는 기본 설정에서도 보드 전원버튼을 누르면 이 기존 타이머가 동작할 수 있다.
- 게임 전용 정리 코드는 이 저장소에 없어 이 프로그램에서는 호출하지 않는다. 게임이 없는 경우 Windows 정상 종료 요청을 접수시키고 같은 보드 응답/전원 처리 흐름으로 들어간다.

## 최초 사용 순서 (사용자가 실행)

1. 승인 후 Main 펌웨어 빌드 및 Connector 복원/빌드. Main 플래시는 사용자가 직접 수행한다.
2. 게시된 BoardWifiConnector.exe 하나를 쓰기 가능한 폴더에 둔다. 기본 INI 양식은 EXE에 포함되어 있으므로 최초 실행하면 옆에 connection.ini를 생성한다.
3. 게임과 TM의 시리얼 연결을 닫고 보드를 USB로 연결한다.
4. 트레이 우클릭 → USB 최초 등록 → Main의 포트를 선택한다. 이 동작이 해당 포트를 잠시 열며 일부 보드는 재부팅할 수 있다.
5. 프로그램은 Windows 사용자 Wi-Fi 프로필을 등록하고 INI의 프로필/어댑터 GUID/보드 식별값을 채운다. 토큰은 현재 사용자 DPAPI로 암호화하여 credential.dat에 저장한다. 비밀번호와 토큰은 로그/INI에 기록하지 않는다.
6. 등록 후 USB 포트를 닫는다. 이후 접속 및 종료 요청은 Wi-Fi/TCP만 사용하므로 게임/TM이 시리얼을 사용할 수 있다.
7. INI의 AllowWindowsShutdown=false는 요청 수신만 하고 Windows를 종료하거나 성공 ACK를 보내지 않는다. 실제 종료를 사용하려면 true로 변경하고 트레이의 재연결 / INI 다시 읽기를 선택한다.
8. 트레이에서 로그온 자동 시작 등록을 선택하면 현재 사용자 Run 항목에 EXE를 등록한다. 등록/해제는 명시적 메뉴 선택 때만 수행된다.

PC 사용자나 보드의 NVS가 변경되면 최초 등록을 다시 수행한다. credential.dat는 다른 사용자/PC로 복사하여 사용할 수 없다. 여러 Wi-Fi 어댑터가 있으면 최초 등록 전에 INI의 WifiInterfaceGuid에 사용할 장치 GUID를 지정한다. 어댑터가 한 개일 때만 최초 등록에서 자동 선택한다.

## 트레이 표시

| 상태 | 색상 |
| --- | --- |
| 시작 직후 / 사용자가 통신 일시 정지 | 회색 |
| Wi-Fi/TCP/인증 연결 또는 최초 등록 중 | 노란색 |
| INI·접속 정보 누락, 연결 실패, 생존 응답 없음, 종료 요청 실패 | 적색 |
| 보드 인증 및 생존 응답 정상 | 녹색 |

Global 이름의 Mutex로 중복 실행을 막는다. 일반 메인 창은 없으며 메뉴에서 INI를 열 때만 메모장을 실행한다. 프로세스 종료와 통신 일시 정지는 Windows 종료를 요청하지 않는다. 오류는 트레이 설명과 상태 메뉴에 표시하며 통신 원문/자격 정보 로그를 만들지 않는다.

## 설정 파일

INI와 암호화 접속 정보는 EXE 폴더의 고정 파일만 읽는다. 현재 작업 디렉터리·명령행 인수로 다른 설정 파일을 선택하거나 다른 보드를 검색하지 않는다.

| 항목 | 의미 |
| --- | --- |
| WifiProfileName | 접속할 Windows 사용자 Wi-Fi 프로필 |
| WifiInterfaceGuid | 전용 통신에 사용할 Wi-Fi 어댑터 |
| BoardAddress / TcpPort | 펌웨어에 지정한 AP 주소 / 포트 |
| BoardId | 인증 응답에서 일치해야 하는 등록 보드 식별값 |
| RetrySeconds | 실패 후 재연결 대기, 1~60초 |
| ResponseTimeoutSeconds | 응답 제한, 3~10초 |
| HeartbeatSeconds | 생존 신호 간격, 1~3초이며 응답 제한보다 작아야 함 |
| AllowWindowsShutdown | 실제 Windows 종료 허용 여부 |

Windows의 WLAN AutoConfig 서비스와 해당 Wi-Fi 프로필을 사용할 수 있어야 한다. Windows 버전/정책에 따라 Wi-Fi API가 위치 접근 허용을 요구할 수 있으며, 거부되면 적색으로 표시한다. 프로그램은 해당 권한이나 Windows의 다른 Wi-Fi 자동 접속 정책을 임의 변경하지 않는다. 한 Wi-Fi 어댑터를 보드 AP에 접속시키면 기존 무선망 연결은 바뀔 수 있으므로 기존 인터넷은 유선 LAN 또는 별도 어댑터로 구성할 수 있다.

## 프로토콜

ASCII 한 줄에 한 메시지, LF 종료, 최대 159자. 비밀번호는 TCP에 보내지 않으며 보드별 토큰으로 최초 인증한다. 일반 shutdown 메시지는 인증된 단일 TCP 클라이언트만 받는다.

- PC → 보드: HELLO 1 + 인증 토큰
- 보드 → PC: READY 1 + 보드 식별값 + 부팅 세션
- PC ↔ 보드: PING / PONG + 임의 생존 신호 식별값
- 보드 → PC: SHUTDOWN + 부팅 세션 + 요청 번호
- PC → 보드: ACCEPT 또는 DECLINE + 부팅 세션 + 요청 번호

Windows가 정상 종료 요청을 접수한 경우에만 ACCEPT를 보낸다. 이 응답은 종료 완료를 뜻하지 않는다. 실제 전원 차단 판단은 기존 USB5V 입력이 담당한다. Windows는 5초 뒤 종료를 시작하며 강제 앱 종료 옵션은 사용하지 않는다. 재전송/재연결 시 같은 세션·요청 번호는 중복 종료 API 호출 없이 ACK만 반복한다. USB 등록 명령 $BWPAIR%는 게임 동작 중 거절하며 정상 명령 카운터와 로그에 포함하지 않는다.

## VS2022 / VS Code

- VS2022: BoardWifiConnector.sln을 연다.
- VS Code: BoardWifiConnector.code-workspace를 연다. C# 디버거 확장이 필요하다.
- 두 IDE는 동일 csproj와 저장소 루트의 고정 SDK global.json을 사용한다.
- Debug 출력: 저장소 루트 bin/BoardWifiConnector/Debug/win-x64.
- Release 게시 출력: 저장소 루트 bin/BoardWifiConnector/Release.

아래 명령은 제공만 했으며 실행하지 않았다. Codex 실행은 별도 승인 필요.

```powershell
.\BoardWifiConnector\build.ps1 -Mode Restore
.\BoardWifiConnector\build.ps1 -Mode Build
.\BoardWifiConnector\build.ps1 -Mode Publish
```

Restore는 최초 NuGet 잠금을 생성/재평가한다. Build/Publish는 기존 잠금으로 복원하며 잠금이 없으면 중단한다. 실제 두 IDE 디버깅 및 산출물은 아직 확인하지 않았다.

## 참고

- [ESP32-S3 Wi-Fi AP/STA 지원](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32s3/api-guides/wifi.html)
- [Windows Wi-Fi 프로필 등록](https://learn.microsoft.com/en-us/windows/win32/api/wlanapi/nf-wlanapi-wlansetprofile)
- [Windows Wi-Fi 연결](https://learn.microsoft.com/en-us/windows/win32/api/wlanapi/nf-wlanapi-wlanconnect)
- [Windows 정상 종료 요청](https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-initiatesystemshutdownexw)
