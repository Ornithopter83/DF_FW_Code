using System.ComponentModel;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Text;

internal enum ConnectorState { Disconnected, Connecting, Failed, Connected }

internal sealed class BoardConnection
{
    private readonly HashSet<string> acceptedRequests = new(StringComparer.Ordinal);
    private readonly Action<ConnectorState, string> report;
    public BoardConnection(Action<ConnectorState, string> report) { this.report = report; }

    public async Task RunAsync(CancellationToken cancellation)
    {
        report(ConnectorState.Disconnected, "미연결");
        while (!cancellation.IsCancellationRequested)
        {
            int retry = 5;
            try
            {
                ConnectorSettings settings = ConnectorSettings.Load();
                retry = settings.RetrySeconds;
                string token = ConnectorSettings.LoadToken();
                report(ConnectorState.Connecting, "지정된 보드에 연결 중");
                using var wifi = new NativeWifi();
                await wifi.ConnectAsync(settings, cancellation);
                using var client = new TcpClient(AddressFamily.InterNetwork) { NoDelay = true };
                using (var timeout = CancellationTokenSource.CreateLinkedTokenSource(cancellation))
                {
                    timeout.CancelAfter(TimeSpan.FromSeconds(settings.TimeoutSeconds));
                    await client.ConnectAsync(settings.BoardAddress, settings.Port, timeout.Token);
                }
                await SessionAsync(client.GetStream(), settings, token, cancellation);
            }
            catch (OperationCanceledException) when (cancellation.IsCancellationRequested) { break; }
            catch (Exception error)
            {
                // Do not expose packet contents, credentials or private endpoint values.
                report(ConnectorState.Failed, FriendlyError(error));
            }
            try { await Task.Delay(TimeSpan.FromSeconds(retry), cancellation); }
            catch (OperationCanceledException) { break; }
        }
    }

    public static string FriendlyError(Exception error) => error switch
    {
        SocketException => "TCP 연결 실패",
        Win32Exception native => "Windows 통신/권한 오류 (" + native.NativeErrorCode + ")",
        FileNotFoundException => "INI 또는 접속 정보 없음: 최초 등록 필요",
        DirectoryNotFoundException => "설정 경로 없음",
        UnauthorizedAccessException => "설정 파일 접근 권한 오류",
        CryptographicException => "접속 정보 복호화 실패: 이 사용자로 재등록 필요",
        FormatException => "설정 또는 통신 형식 오류",
        InvalidOperationException => "Wi-Fi 어댑터 선택 또는 상태 확인 필요",
        TimeoutException or OperationCanceledException => "연결 또는 응답 시간 초과",
        IOException => "연결 끊김 또는 파일 읽기 실패",
        _ => "연결 처리 오류"
    };

    private async Task SessionAsync(NetworkStream stream, ConnectorSettings settings, string token, CancellationToken cancellation)
    {
        var reader = new BoundedLineReader(stream);
        await WriteAsync(stream, "HELLO 1 " + token, settings.TimeoutSeconds, cancellation);
        string[] ready = (await reader.ReadAsync(settings.TimeoutSeconds, cancellation)).Split(' ');
        if (ready.Length != 4 || ready[0] != "READY" || ready[1] != "1" || ready[2] != settings.BoardId ||
            !ConnectorSettings.IsHex(ready[3], 16)) throw new FormatException("보드 인증 실패");
        string boot = ready[3];
        report(ConnectorState.Connected, settings.AllowShutdown ? "연결됨" : "연결됨 · Windows 종료 미허용");
        using var sessionCancellation = CancellationTokenSource.CreateLinkedTokenSource(cancellation);
        CancellationToken sessionToken = sessionCancellation.Token;
        Task<string> incoming = reader.ReadAsync(settings.TimeoutSeconds, sessionToken);
        string? nonce = null;
        long nextPing = Environment.TickCount64;
        long pongDeadline = 0;
        try
        {
            while (true)
            {
                sessionToken.ThrowIfCancellationRequested();
                long now = Environment.TickCount64;
                if (nonce == null && now >= nextPing)
                {
                    nonce = Convert.ToHexString(RandomNumberGenerator.GetBytes(8)).ToLowerInvariant();
                    await WriteAsync(stream, "PING " + nonce, settings.TimeoutSeconds, sessionToken);
                    pongDeadline = Environment.TickCount64 + settings.TimeoutSeconds * 1000;
                }
                long wakeAt = nonce == null ? nextPing : pongDeadline;
                if (nonce != null && now >= pongDeadline) throw new TimeoutException();
                int wait = (int)Math.Max(1, wakeAt - Environment.TickCount64);
                Task wake = Task.Delay(wait, sessionToken);
                if (await Task.WhenAny(incoming, wake) != incoming) continue;
                string line = await incoming;
                incoming = reader.ReadAsync(settings.TimeoutSeconds, sessionToken);
                if (nonce != null && line == "PONG " + nonce)
                {
                    nonce = null;
                    nextPing = Environment.TickCount64 + settings.HeartbeatSeconds * 1000;
                    continue;
                }
                await HandleShutdownAsync(line, boot, stream, settings, sessionToken);
            }
        }
        finally
        {
            sessionCancellation.Cancel();
            try { await incoming; } catch { }
        }
    }

    private async Task HandleShutdownAsync(string line, string boot, NetworkStream stream,
        ConnectorSettings settings, CancellationToken cancellation)
    {
        string[] fields = line.Split(' ');
        if (fields.Length != 3 || fields[0] != "SHUTDOWN" || fields[1] != boot ||
            !uint.TryParse(fields[2], System.Globalization.NumberStyles.None,
                System.Globalization.CultureInfo.InvariantCulture, out uint request) || request == 0)
            throw new FormatException("예상하지 않은 보드 응답");
        if (!settings.AllowShutdown)
        {
            report(ConnectorState.Connected, "종료 요청 수신 · 설정에서 종료 미허용");
            await WriteAsync(stream, "DECLINE " + boot + " " + fields[2], settings.TimeoutSeconds, cancellation);
            return;
        }
        string key = boot + ":" + fields[2];
        if (!acceptedRequests.Contains(key))
        {
            try { WindowsShutdown.Request(); }
            catch (Win32Exception error)
            {
                report(ConnectorState.Failed, "Windows 종료 요청 실패 (" + error.NativeErrorCode + ")");
                await WriteAsync(stream, "DECLINE " + boot + " " + fields[2], settings.TimeoutSeconds, cancellation);
                return;
            }
            // Repeated packets and reconnects only repeat ACK, not the OS request.
            if (acceptedRequests.Count >= 32) acceptedRequests.Clear();
            acceptedRequests.Add(key);
        }
        await WriteAsync(stream, "ACCEPT " + boot + " " + fields[2], settings.TimeoutSeconds, cancellation);
        report(ConnectorState.Connected, "Windows 종료 요청 접수됨");
    }

    private static async Task WriteAsync(NetworkStream stream, string message, int seconds, CancellationToken cancellation)
    {
        using var timeout = CancellationTokenSource.CreateLinkedTokenSource(cancellation);
        timeout.CancelAfter(TimeSpan.FromSeconds(seconds));
        await stream.WriteAsync(Encoding.ASCII.GetBytes(message + "\n"), timeout.Token);
    }

    private sealed class BoundedLineReader
    {
        private readonly NetworkStream stream;
        private readonly byte[] buffer = new byte[512];
        private int offset;
        private int length;
        public BoundedLineReader(NetworkStream stream) { this.stream = stream; }
        public Task<string> ReadAsync(int seconds, CancellationToken cancellation) => ReadMillisecondsAsync(seconds * 1000, cancellation);
        public async Task<string> ReadMillisecondsAsync(int milliseconds, CancellationToken cancellation)
        {
            using var timeout = CancellationTokenSource.CreateLinkedTokenSource(cancellation);
            timeout.CancelAfter(milliseconds);
            var line = new StringBuilder(160);
            while (true)
            {
                if (offset == length)
                {
                    length = await stream.ReadAsync(buffer.AsMemory(), timeout.Token);
                    offset = 0;
                    if (length == 0) throw new IOException("연결 종료");
                }
                byte character = buffer[offset++];
                if (character == '\r') continue;
                if (character == '\n') return line.ToString();
                if (character < 32 || character > 126 || line.Length >= 159) throw new FormatException("수신 길이 오류");
                line.Append((char)character);
            }
        }
    }
}
