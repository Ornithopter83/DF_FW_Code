using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Xml.Linq;

internal sealed class NativeWifi : IDisposable
{
    private IntPtr handle;
    public NativeWifi()
    {
        Check(WlanOpenHandle(2, IntPtr.Zero, out _, out handle));
    }
    private static void Check(uint result) { if (result != 0) throw new Win32Exception((int)result); }

    public Guid SelectInterface(Guid? configured)
    {
        Check(WlanEnumInterfaces(handle, IntPtr.Zero, out IntPtr list));
        try
        {
            int count = Marshal.ReadInt32(list);
            int size = Marshal.SizeOf<InterfaceInfo>();
            var candidates = new List<Guid>();
            for (int index = 0; index < count; ++index)
                candidates.Add(Marshal.PtrToStructure<InterfaceInfo>(IntPtr.Add(list, 8 + index * size)).Id);
            if (configured.HasValue && candidates.Contains(configured.Value)) return configured.Value;
            if (!configured.HasValue && candidates.Count == 1) return candidates[0];
            throw new InvalidOperationException("Wi-Fi 어댑터 선택 필요");
        }
        finally { WlanFreeMemory(list); }
    }

    public void RegisterProfile(Guid adapter, string ssid, string password)
    {
        XNamespace ns = "http://www.microsoft.com/networking/WLAN/profile/v1";
        var profile = new XElement(ns + "WLANProfile",
            new XElement(ns + "name", ssid),
            new XElement(ns + "SSIDConfig", new XElement(ns + "SSID", new XElement(ns + "name", ssid))),
            new XElement(ns + "connectionType", "ESS"),
            new XElement(ns + "connectionMode", "manual"),
            new XElement(ns + "MSM", new XElement(ns + "security",
                new XElement(ns + "authEncryption", new XElement(ns + "authentication", "WPA2PSK"),
                    new XElement(ns + "encryption", "AES"), new XElement(ns + "useOneX", "false")),
                new XElement(ns + "sharedKey", new XElement(ns + "keyType", "passPhrase"),
                    new XElement(ns + "protected", "false"), new XElement(ns + "keyMaterial", password)))));
        // XML remains in memory; Windows encrypts the saved user profile's key.
        Check(WlanSetProfile(handle, ref adapter, 2, profile.ToString(SaveOptions.DisableFormatting),
            null, true, IntPtr.Zero, out _));
    }

    private bool HasConnection(Guid adapter, string profile)
    {
        // current_connection; no SSID discovery or alternate-profile selection.
        uint result = WlanQueryInterface(handle, ref adapter, 7, IntPtr.Zero, out uint length, out IntPtr data, out _);
        if (result == 5023 || result == 1168) return false;
        Check(result);
        try
        {
            return length >= 520 && Marshal.ReadInt32(data) == 1 &&
                Marshal.PtrToStringUni(IntPtr.Add(data, 8), 256)?.TrimEnd('\0') == profile;
        }
        finally { WlanFreeMemory(data); }
    }

    public async Task ConnectAsync(ConnectorSettings settings, CancellationToken cancellation)
    {
        Guid adapter = SelectInterface(settings.InterfaceId);
        if (HasConnection(adapter, settings.ProfileName)) return;
        var parameters = new ConnectionParameters { Mode = 0, Profile = settings.ProfileName, BssType = 1 };
        Check(WlanConnect(handle, ref adapter, ref parameters, IntPtr.Zero));
        long deadline = Environment.TickCount64 + 15000;
        while (Environment.TickCount64 < deadline)
        {
            cancellation.ThrowIfCancellationRequested();
            if (HasConnection(adapter, settings.ProfileName)) return;
            await Task.Delay(250, cancellation);
        }
        throw new TimeoutException("Wi-Fi 연결 시간 초과");
    }

    public void Dispose() { if (handle != IntPtr.Zero) { WlanCloseHandle(handle, IntPtr.Zero); handle = IntPtr.Zero; } }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    private struct InterfaceInfo
    {
        public Guid Id;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)] public string Description;
        public int State;
    }
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    private struct ConnectionParameters
    {
        public int Mode;
        [MarshalAs(UnmanagedType.LPWStr)] public string Profile;
        public IntPtr Ssid;
        public IntPtr BssidList;
        public int BssType;
        public uint Flags;
    }
    [DllImport("wlanapi.dll")] private static extern uint WlanOpenHandle(uint version, IntPtr reserved, out uint negotiated, out IntPtr client);
    [DllImport("wlanapi.dll")] private static extern uint WlanCloseHandle(IntPtr client, IntPtr reserved);
    [DllImport("wlanapi.dll")] private static extern uint WlanEnumInterfaces(IntPtr client, IntPtr reserved, out IntPtr list);
    [DllImport("wlanapi.dll")] private static extern void WlanFreeMemory(IntPtr memory);
    [DllImport("wlanapi.dll", CharSet = CharSet.Unicode)] private static extern uint WlanSetProfile(IntPtr client, ref Guid adapter,
        uint flags, string xml, string? security, [MarshalAs(UnmanagedType.Bool)] bool overwrite, IntPtr reserved, out uint reason);
    [DllImport("wlanapi.dll")] private static extern uint WlanConnect(IntPtr client, ref Guid adapter, ref ConnectionParameters parameters, IntPtr reserved);
    [DllImport("wlanapi.dll")] private static extern uint WlanQueryInterface(IntPtr client, ref Guid adapter, int opcode, IntPtr reserved,
        out uint size, out IntPtr data, out int valueType);
}
