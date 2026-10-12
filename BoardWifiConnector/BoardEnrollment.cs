using System.IO.Ports;
using System.Text;

internal static class BoardEnrollment
{
    public static void Pair(string portName, CancellationToken cancellation)
    {
        Guid? adapter = null;
        var ini = ConnectorSettings.ReadIni();
        if (ini.TryGetValue("WifiInterfaceGuid", out string? value) && value.Length != 0)
            adapter = Guid.TryParse(value, out Guid id) ? id : throw new FormatException("어댑터 GUID 오류");
        using var wifi = new NativeWifi();
        Guid selected = wifi.SelectInterface(adapter);
        using var serial = new SerialPort(portName, 115200)
        {
            ReadTimeout = 250, WriteTimeout = 1000, DtrEnable = false, RtsEnable = false,
            Encoding = Encoding.ASCII
        };
        serial.Open();
        // Some bridges reset the ESP on open. Wait without issuing game commands.
        if (cancellation.WaitHandle.WaitOne(2000)) cancellation.ThrowIfCancellationRequested();
        serial.DiscardInBuffer();
        var frame = new StringBuilder();
        long deadline = Environment.TickCount64 + 15000;
        long nextRequest = 0;
        while (Environment.TickCount64 < deadline)
        {
            cancellation.ThrowIfCancellationRequested();
            if (Environment.TickCount64 >= nextRequest)
            {
                serial.Write("$BWPAIR%");
                nextRequest = Environment.TickCount64 + 2000;
            }
            int character;
            try { character = serial.ReadChar(); }
            catch (TimeoutException) { continue; }
            if (character == '$') { frame.Clear(); frame.Append('$'); }
            else if (character == '%')
            {
                string[] fields = frame.ToString().Split(',');
                frame.Clear();
                if (fields.Length == 2 && fields[0] == "$BWPAIR" && fields[1] == "BUSY")
                    continue;
                if (fields.Length != 5 || fields[0] != "$BWPAIR") continue;
                if (!ConnectorSettings.IsHex(fields[4], 16) || fields[1] != "DF-Board-" + fields[4] ||
                    !ConnectorSettings.IsHex(fields[2], 32) || !ConnectorSettings.IsHex(fields[3], 64))
                    throw new FormatException("보드 등록 응답 형식 오류");
                // Never write the received frame, password or token into diagnostics.
                wifi.RegisterProfile(selected, fields[1], fields[2]);
                ConnectorSettings.SaveEnrollment(fields[1], selected, fields[4], fields[3]);
                return;
            }
            else if (frame.Length > 0 && character >= 32 && character <= 126)
            {
                frame.Append((char)character);
                if (frame.Length > 256) frame.Clear();
            }
        }
        throw new TimeoutException("등록 응답 없음: 게임 종료·USB 포트·펌웨어 확인 필요");
    }
}
