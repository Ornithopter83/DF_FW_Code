using System.Drawing;

namespace DFTestModule;

internal sealed class BatteryGauge : Control
{
    private int percent = -1;
    private int averageAdc = -1;
    private string batteryState = "";

    public BatteryGauge()
    {
        SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint |
            ControlStyles.OptimizedDoubleBuffer | ControlStyles.ResizeRedraw, true);
        BackColor = Color.FromArgb(247, 248, 250);
        ForeColor = Color.FromArgb(35, 45, 55);
    }

    public int Percent
    {
        get { return percent; }
        set
        {
            int next = value < 0 || value > 99 ? -1 : value;
            if (percent == next) return;
            percent = next;
            Invalidate();
        }
    }

    public int AverageAdc
    {
        get { return averageAdc; }
        set
        {
            int next = value < 0 || value > 4095 ? -1 : value;
            if (averageAdc == next) return;
            averageAdc = next;
            Invalidate();
        }
    }

    public string BatteryState
    {
        get { return batteryState; }
        set
        {
            string next = value ?? "";
            if (batteryState == next) return;
            batteryState = next;
            Invalidate();
        }
    }

    protected override void OnPaint(PaintEventArgs e)
    {
        base.OnPaint(e);

        Rectangle bounds = ClientRectangle;
        if (bounds.Width < 3 || bounds.Height < 3) return;

        using (SolidBrush trackBrush = new(Color.FromArgb(224, 229, 234)))
        using (SolidBrush fillBrush = new(Color.FromArgb(102, 211, 117)))
        using (Pen borderPen = new(Color.FromArgb(175, 184, 193)))
        {
            e.Graphics.FillRectangle(trackBrush, bounds);
            if (percent > 0)
            {
                int fillWidth = (bounds.Width - 1) * percent / 99;
                e.Graphics.FillRectangle(fillBrush, 1, 1, fillWidth, bounds.Height - 2);
            }
            e.Graphics.DrawRectangle(borderPen, 0, 0, bounds.Width - 1, bounds.Height - 1);
        }

        string text = percent < 0 ? "--%" : percent.ToString() + "%";
        if (batteryState.Length > 0) text += " (" + batteryState + ")";
        text += averageAdc < 0 ? " (----)" : " (" + averageAdc.ToString("D4") + ")";
        TextRenderer.DrawText(e.Graphics, text, Font, bounds, ForeColor,
            TextFormatFlags.HorizontalCenter | TextFormatFlags.VerticalCenter |
            TextFormatFlags.SingleLine | TextFormatFlags.NoPadding);
    }
}
