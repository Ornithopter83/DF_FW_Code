using System.Drawing.Drawing2D;
using System.Globalization;

namespace DFTestModule;

internal sealed class ImuOrientationView : Control
{
    private readonly Point3[] model =
    {
        new(-0.90f, -0.62f, -0.55f), new(0.90f, -0.62f, -0.55f),
        new(0.90f, 0.62f, -0.55f), new(-0.90f, 0.62f, -0.55f),
        new(-0.90f, -0.62f, 0.55f), new(0.90f, -0.62f, 0.55f),
        new(0.90f, 0.62f, 0.55f), new(-0.90f, 0.62f, 0.55f)
    };

    private readonly Face[] faces =
    {
        new(new[] { 0, 1, 2, 3 }, Color.FromArgb(168, 178, 190), ""),
        new(new[] { 4, 5, 6, 7 }, Color.FromArgb(205, 183, 145), ""),
        new(new[] { 0, 4, 7, 3 }, Color.FromArgb(159, 176, 194), ""),
        new(new[] { 1, 5, 6, 2 }, Color.FromArgb(176, 188, 199), ""),
        new(new[] { 0, 1, 5, 4 }, Color.FromArgb(151, 164, 179), ""),
        new(new[] { 3, 2, 6, 7 }, Color.FromArgb(220, 205, 164), "")
    };

    private readonly Point3 camera = new(3.2f, 2.4f, 6.5f);
    private readonly Point3 cameraForward;
    private readonly Point3 cameraRight;
    private readonly Point3 cameraUp;
    private Matrix3 sensorOrientation = Matrix3.Identity;
    private Matrix3 zeroOrientation = Matrix3.Identity;
    private float roll;
    private float pitch;
    private float yaw;
    private float zeroRoll;
    private float zeroPitch;
    private float zeroYaw;
    private bool hasData;
    private bool hasZero;

    public ImuOrientationView()
    {
        DoubleBuffered = true;
        SetStyle(ControlStyles.ResizeRedraw, true);
        BackColor = Color.FromArgb(247, 248, 250);
        cameraForward = Normalize(new Point3(-camera.X, -camera.Y, -camera.Z));
        cameraRight = Normalize(Cross(cameraForward, new Point3(0, 1, 0)));
        cameraUp = Normalize(Cross(cameraRight, cameraForward));
    }

    public void SetAngles(float newRoll, float newPitch, float newYaw)
    {
        roll = newRoll;
        pitch = newPitch;
        yaw = newYaw;
        sensorOrientation = CreateOrientation(roll, pitch, yaw);
        hasData = true;
        Invalidate();
    }

    public bool SetCurrentAsZero()
    {
        if (!hasData) return false;
        zeroOrientation = sensorOrientation;
        zeroRoll = roll;
        zeroPitch = pitch;
        zeroYaw = yaw;
        hasZero = true;
        Invalidate();
        return true;
    }

    public void ClearZero()
    {
        hasZero = false;
        hasData = false;
        zeroOrientation = Matrix3.Identity;
        Invalidate();
    }

    protected override void OnPaint(PaintEventArgs e)
    {
        base.OnPaint(e);
        e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
        DrawReference(e.Graphics);

        float shownRoll = hasZero ? Wrap180(roll - zeroRoll) : roll;
        float shownPitch = hasZero ? Wrap180(pitch - zeroPitch) : pitch;
        float shownYaw = hasZero ? Wrap180(yaw - zeroYaw) : yaw;
        Matrix3 displayOrientation = CreateOrientation(shownRoll, shownPitch, shownYaw);
        Point3[] rotated = model.Select(point => displayOrientation.Transform(point)).ToArray();
        ProjectedPoint[] projected = rotated.Select(Project).ToArray();
        Face[] orderedFaces = faces.OrderByDescending(face => face.Indices.Average(index => projected[index].Depth)).ToArray();

        using Pen edge = new(Color.FromArgb(45, 53, 62), 1.8f);
        using Font faceFont = new("맑은 고딕", 8.5F, FontStyle.Bold);
        using Brush textBrush = new SolidBrush(Color.FromArgb(35, 39, 45));
        using StringFormat centered = new() { Alignment = StringAlignment.Center, LineAlignment = StringAlignment.Center };
        foreach (Face face in orderedFaces)
        {
            PointF[] polygon = face.Indices.Select(index => projected[index].Screen).ToArray();
            using Brush fill = new SolidBrush(Color.FromArgb(225, face.Color));
            e.Graphics.FillPolygon(fill, polygon);
            e.Graphics.DrawPolygon(edge, polygon);
            if (face.Label.Length > 0)
            {
                float x = polygon.Average(point => point.X);
                float y = polygon.Average(point => point.Y);
                e.Graphics.DrawString(face.Label, faceFont, textBrush, new PointF(x, y), centered);
            }
        }

        DrawAxes(e.Graphics, displayOrientation);

        using Font angleFont = new("Consolas", 8F);
        using Brush angleBrush = new SolidBrush(Color.DimGray);
        string prefix = hasZero ? "ZERO  " : "RAW   ";
        e.Graphics.DrawString(string.Format(CultureInfo.InvariantCulture,
            "{0}R {1,6:0.0}  P {2,6:0.0}  Y {3,6:0.0}", prefix, shownRoll, shownPitch, shownYaw),
            angleFont, angleBrush, 5, Height - 19);
    }

    private void DrawReference(Graphics graphics)
    {
        int baseline = Height - 29;
        using Pen line = new(Color.FromArgb(218, 221, 225), 1F);
        graphics.DrawLine(line, 18, baseline, Width - 18, baseline);
        using Brush shadow = new SolidBrush(Color.FromArgb(22, 35, 40, 48));
        graphics.FillEllipse(shadow, Width * 0.22f, baseline - 8, Width * 0.58f, 15);
    }

    private void DrawAxes(Graphics graphics, Matrix3 orientation)
    {
        ProjectedPoint center = Project(orientation.Transform(new Point3(0, 0, 0)));
        DrawAxis(graphics, center.Screen, Project(orientation.Transform(new Point3(0, 0, -1.45f))).Screen,
            Color.FromArgb(210, 45, 45), "FRONT");
        DrawAxis(graphics, center.Screen, Project(orientation.Transform(new Point3(0, 1.25f, 0))).Screen,
            Color.FromArgb(45, 105, 220), "TOP");
        DrawAxis(graphics, center.Screen, Project(orientation.Transform(new Point3(1.35f, 0, 0))).Screen,
            Color.FromArgb(35, 155, 75), "RIGHT");
    }

    private static void DrawAxis(Graphics graphics, PointF start, PointF end, Color color, string label)
    {
        using Pen outline = new(Color.FromArgb(210, Color.White), 7.0f) { StartCap = LineCap.Round, EndCap = LineCap.ArrowAnchor };
        using Pen arrow = new(color, 4.5f) { StartCap = LineCap.Round, EndCap = LineCap.ArrowAnchor };
        graphics.DrawLine(outline, start, end);
        graphics.DrawLine(arrow, start, end);
        using Font font = new("맑은 고딕", 7.5F, FontStyle.Bold);
        using Brush brush = new SolidBrush(color);
        float labelX = end.X + (end.X >= start.X ? 4 : -34);
        float labelY = end.Y + (end.Y >= start.Y ? 3 : -15);
        graphics.DrawString(label, font, brush, labelX, labelY);
    }

    private ProjectedPoint Project(Point3 point)
    {
        Point3 relative = new(point.X - camera.X, point.Y - camera.Y, point.Z - camera.Z);
        float viewX = Dot(relative, cameraRight);
        float viewY = Dot(relative, cameraUp);
        float depth = Math.Max(0.4f, Dot(relative, cameraForward));
        float focal = Math.Min(Width, Height) * 2.25f;
        float centerX = Width * 0.50f;
        float centerY = Height * 0.46f;
        return new ProjectedPoint(new PointF(centerX + focal * viewX / depth, centerY - focal * viewY / depth), depth);
    }

    private static Matrix3 CreateOrientation(float rollDegrees, float pitchDegrees, float yawDegrees)
    {
        float roll = DegreesToRadians(-rollDegrees);
        float pitch = DegreesToRadians(pitchDegrees);
        float yaw = DegreesToRadians(-yawDegrees);
        return Matrix3.RotateY(yaw) * Matrix3.RotateX(pitch) * Matrix3.RotateZ(roll);
    }

    private static float DegreesToRadians(float value) => value * MathF.PI / 180.0f;
    private static float Wrap180(float value)
    {
        while (value > 180.0f) value -= 360.0f;
        while (value <= -180.0f) value += 360.0f;
        return value;
    }

    private static float Dot(Point3 left, Point3 right) => left.X * right.X + left.Y * right.Y + left.Z * right.Z;
    private static Point3 Cross(Point3 left, Point3 right) => new(
        left.Y * right.Z - left.Z * right.Y,
        left.Z * right.X - left.X * right.Z,
        left.X * right.Y - left.Y * right.X);
    private static Point3 Normalize(Point3 point)
    {
        float length = MathF.Sqrt(Dot(point, point));
        return new Point3(point.X / length, point.Y / length, point.Z / length);
    }

    private readonly record struct Point3(float X, float Y, float Z);
    private readonly record struct ProjectedPoint(PointF Screen, float Depth);
    private sealed record Face(int[] Indices, Color Color, string Label);

    private readonly record struct Matrix3(
        float M11, float M12, float M13,
        float M21, float M22, float M23,
        float M31, float M32, float M33)
    {
        public static Matrix3 Identity => new(1, 0, 0, 0, 1, 0, 0, 0, 1);

        public Point3 Transform(Point3 point) => new(
            M11 * point.X + M12 * point.Y + M13 * point.Z,
            M21 * point.X + M22 * point.Y + M23 * point.Z,
            M31 * point.X + M32 * point.Y + M33 * point.Z);

        public static Matrix3 RotateX(float angle)
        {
            float c = MathF.Cos(angle), s = MathF.Sin(angle);
            return new Matrix3(1, 0, 0, 0, c, -s, 0, s, c);
        }

        public static Matrix3 RotateY(float angle)
        {
            float c = MathF.Cos(angle), s = MathF.Sin(angle);
            return new Matrix3(c, 0, s, 0, 1, 0, -s, 0, c);
        }

        public static Matrix3 RotateZ(float angle)
        {
            float c = MathF.Cos(angle), s = MathF.Sin(angle);
            return new Matrix3(c, -s, 0, s, c, 0, 0, 0, 1);
        }

        public static Matrix3 Transpose(Matrix3 value) => new(
            value.M11, value.M21, value.M31,
            value.M12, value.M22, value.M32,
            value.M13, value.M23, value.M33);

        public static Matrix3 operator *(Matrix3 left, Matrix3 right) => new(
            left.M11 * right.M11 + left.M12 * right.M21 + left.M13 * right.M31,
            left.M11 * right.M12 + left.M12 * right.M22 + left.M13 * right.M32,
            left.M11 * right.M13 + left.M12 * right.M23 + left.M13 * right.M33,
            left.M21 * right.M11 + left.M22 * right.M21 + left.M23 * right.M31,
            left.M21 * right.M12 + left.M22 * right.M22 + left.M23 * right.M32,
            left.M21 * right.M13 + left.M22 * right.M23 + left.M23 * right.M33,
            left.M31 * right.M11 + left.M32 * right.M21 + left.M33 * right.M31,
            left.M31 * right.M12 + left.M32 * right.M22 + left.M33 * right.M32,
            left.M31 * right.M13 + left.M32 * right.M23 + left.M33 * right.M33);
    }
}
