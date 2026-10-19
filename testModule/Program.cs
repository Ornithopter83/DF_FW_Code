namespace DFTestModule;

internal static class Program
{
    [STAThread]
    private static int Main(string[] args)
    {
        if (args.Length == 1 && string.Equals(args[0], "--self-test", StringComparison.Ordinal))
            return SelfTest.Run();

        using var singleInstanceMutex = new Mutex(
            initiallyOwned: true,
            name: @"Local\DFTestModule_SingleInstance",
            createdNew: out bool isFirstInstance);

        if (!isFirstInstance)
        {
            MessageBox.Show(
                "DF TestModule이 이미 실행 중입니다.",
                "DF TestModule",
                MessageBoxButtons.OK,
                MessageBoxIcon.Information);
            return 0;
        }

        try
        {
            ApplicationConfiguration.Initialize();
            Application.Run(new MainForm());
        }
        finally
        {
            singleInstanceMutex.ReleaseMutex();
        }

        return 0;
    }
}

