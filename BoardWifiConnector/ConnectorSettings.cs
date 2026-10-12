using System.Globalization;
using System.Net;
using System.Security.Cryptography;
using System.Text;

internal sealed record ConnectorSettings(string ProfileName, Guid InterfaceId, IPAddress BoardAddress,
    int Port, string BoardId, int RetrySeconds, int TimeoutSeconds, int HeartbeatSeconds, bool AllowShutdown)
{
    // Neither working directory nor command-line arguments can redirect these paths.
    public static string IniPath => Path.Combine(AppContext.BaseDirectory, "connection.ini");
    public static string CredentialPath => Path.Combine(AppContext.BaseDirectory, "credential.dat");

    public static Dictionary<string, string> ReadIni()
    {
        var values = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        foreach (string raw in File.ReadAllLines(IniPath))
        {
            string line = raw.Trim();
            if (line.Length == 0 || line.StartsWith(';') || line.StartsWith('#') || line.StartsWith('[')) continue;
            int separator = line.IndexOf('=');
            if (separator <= 0) throw new FormatException("INI 형식 오류");
            string key = line[..separator].Trim();
            if (!values.TryAdd(key, line[(separator + 1)..].Trim())) throw new FormatException("INI 중복 항목");
        }
        return values;
    }

    public static ConnectorSettings Load()
    {
        var values = ReadIni();
        string Required(string key) => values.TryGetValue(key, out string? value) && value.Length != 0
            ? value : throw new FormatException("INI 필수 항목 누락");
        int Number(string key, int min, int max)
        {
            if (!int.TryParse(Required(key), NumberStyles.None, CultureInfo.InvariantCulture, out int value) || value < min || value > max)
                throw new FormatException("INI 숫자 범위 오류");
            return value;
        }
        string profile = Required("WifiProfileName");
        if (profile.Length > 255) throw new FormatException("Wi-Fi 프로필 이름 오류");
        if (!Guid.TryParse(Required("WifiInterfaceGuid"), out Guid adapter) || adapter == Guid.Empty)
            throw new FormatException("Wi-Fi 어댑터 설정 필요");
        if (!IPAddress.TryParse(Required("BoardAddress"), out IPAddress? address) ||
            address.AddressFamily != System.Net.Sockets.AddressFamily.InterNetwork)
            throw new FormatException("IPv4 주소 설정 필요");
        string id = Required("BoardId");
        if (!IsHex(id, 16)) throw new FormatException("보드 최초 등록 필요");
        int timeout = Number("ResponseTimeoutSeconds", 3, 10);
        int heartbeat = Number("HeartbeatSeconds", 1, 3);
        if (heartbeat >= timeout) throw new FormatException("생존 신호 간격 설정 오류");
        if (!bool.TryParse(Required("AllowWindowsShutdown"), out bool allow)) throw new FormatException("종료 허용 설정 오류");
        return new(profile, adapter, address, Number("TcpPort", 1, 65535), id,
            Number("RetrySeconds", 1, 60), timeout, heartbeat, allow);
    }

    public static bool IsHex(string value, int length) => value.Length == length &&
        value.All(c => c is >= '0' and <= '9' or >= 'a' and <= 'f');

    public static void SaveEnrollment(string profile, Guid adapter, string boardId, string token)
    {
        var values = ReadIni();
        values["WifiProfileName"] = profile;
        values["WifiInterfaceGuid"] = adapter.ToString();
        values["BoardId"] = boardId;
        byte[] clear = Encoding.ASCII.GetBytes(token);
        try
        {
            byte[] encrypted = ProtectedData.Protect(clear, Encoding.ASCII.GetBytes("DF.BoardWifiConnector.v1"), DataProtectionScope.CurrentUser);
            string temporary = CredentialPath + ".tmp";
            File.WriteAllBytes(temporary, encrypted);
            File.Move(temporary, CredentialPath, true);
        }
        finally { CryptographicOperations.ZeroMemory(clear); }
        string iniTemporary = IniPath + ".tmp";
        File.WriteAllLines(iniTemporary, new[] { "[Connection]" }.Concat(values.Select(v => v.Key + "=" + v.Value)), new UTF8Encoding(false));
        File.Move(iniTemporary, IniPath, true);
    }

    public static string LoadToken()
    {
        byte[] clear = ProtectedData.Unprotect(File.ReadAllBytes(CredentialPath),
            Encoding.ASCII.GetBytes("DF.BoardWifiConnector.v1"), DataProtectionScope.CurrentUser);
        try
        {
            string token = Encoding.ASCII.GetString(clear);
            if (!IsHex(token, 64)) throw new FormatException("접속 정보 재등록 필요");
            return token;
        }
        finally { CryptographicOperations.ZeroMemory(clear); }
    }
}
