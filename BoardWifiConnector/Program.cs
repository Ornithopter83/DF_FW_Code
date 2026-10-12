internal static class Program
{
    [STAThread]
    private static void Main()
    {
        // Global name also prevents a second tray process in another login session.
        Mutex instance;
        bool first;
        try { instance = new Mutex(true, @"Global\DF.BoardWifiConnector", out first); }
        catch (UnauthorizedAccessException) { return; }
        using (instance)
        {
            if (!first) return;
            try
            {
                Application.SetHighDpiMode(HighDpiMode.PerMonitorV2);
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                using var context = new ConnectorTrayContext();
                Application.Run(context);
            }
            finally { instance.ReleaseMutex(); }
        }
    }
}
