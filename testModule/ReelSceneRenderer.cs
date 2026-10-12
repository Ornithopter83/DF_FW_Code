using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

namespace DFTestModule;

// A small software depth buffer keeps hidden component edges out of the reel view.
internal sealed class ReelSceneRenderer
{
    private const int SampleScale = 2;
    private const float EdgeDepthBias = 0.0025f;
    private readonly int width;
    private readonly int height;
    private readonly int displayWidth;
    private readonly int displayHeight;
    private readonly int[] colors;
    private readonly float[] depths;

    public ReelSceneRenderer(int width, int height)
    {
        displayWidth = Math.Max(1, width);
        displayHeight = Math.Max(1, height);
        this.width = displayWidth * SampleScale;
        this.height = displayHeight * SampleScale;
        colors = new int[this.width * this.height];
        depths = new float[colors.Length];
        Array.Fill(depths, float.PositiveInfinity);
    }

    public void FillPolygon(PointF[] polygon, float[] depth, Color color)
    {
        int opaque = Color.FromArgb(255, color.R, color.G, color.B).ToArgb();
        for (int i = 1; i < polygon.Length - 1; i++)
            FillTriangle(Vertex(polygon[0], depth[0]), Vertex(polygon[i], depth[i]),
                Vertex(polygon[i + 1], depth[i + 1]), opaque, null, 1.0f);
    }

    public void DrawTexture(PointF[] polygon, float[] depth, ReelSceneTexture texture,
        RectangleF source, float opacity)
    {
        RasterVertex[] vertices =
        {
            Vertex(polygon[0], depth[0], source.Left, source.Top),
            Vertex(polygon[1], depth[1], source.Right, source.Top),
            Vertex(polygon[2], depth[2], source.Right, source.Bottom),
            Vertex(polygon[3], depth[3], source.Left, source.Bottom)
        };
        FillTriangle(vertices[0], vertices[1], vertices[2], 0, texture, opacity);
        FillTriangle(vertices[0], vertices[2], vertices[3], 0, texture, opacity);
    }

    public void StrokePolygon(PointF[] polygon, float[] depth, Color color, float lineWidth)
    {
        for (int i = 0; i < polygon.Length; i++)
        {
            int next = (i + 1) % polygon.Length;
            DrawLine(polygon[i], depth[i], polygon[next], depth[next], color.ToArgb(), lineWidth);
        }
    }

    public bool IsVisible(PointF point, float depth)
    {
        int x = (int)MathF.Floor(point.X * SampleScale);
        int y = (int)MathF.Floor(point.Y * SampleScale);
        return x >= 0 && x < width && y >= 0 && y < height &&
            float.IsFinite(depths[y * width + x]) && depth <= depths[y * width + x] + EdgeDepthBias;
    }

    public void DrawTo(Graphics graphics)
    {
        using Bitmap bitmap = new(width, height, PixelFormat.Format32bppArgb);
        BitmapData data = bitmap.LockBits(new Rectangle(0, 0, width, height),
            ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
        try
        {
            for (int y = 0; y < height; y++)
                Marshal.Copy(colors, y * width, IntPtr.Add(data.Scan0, y * data.Stride), width);
        }
        finally { bitmap.UnlockBits(data); }
        GraphicsState state = graphics.Save();
        try
        {
            graphics.InterpolationMode = InterpolationMode.HighQualityBilinear;
            graphics.PixelOffsetMode = PixelOffsetMode.HighQuality;
            graphics.DrawImage(bitmap, new Rectangle(0, 0, displayWidth, displayHeight),
                0, 0, width, height, GraphicsUnit.Pixel);
        }
        finally { graphics.Restore(state); }
    }

    private static RasterVertex Vertex(PointF point, float depth, float u = 0, float v = 0)
    {
        float inverse = 1.0f / depth;
        return new RasterVertex(point.X * SampleScale, point.Y * SampleScale,
            inverse, u * inverse, v * inverse);
    }

    private void FillTriangle(RasterVertex a, RasterVertex b, RasterVertex c,
        int solidColor, ReelSceneTexture? texture, float opacity)
    {
        float area = Edge(a, b, c.X, c.Y);
        if (!float.IsFinite(area) || MathF.Abs(area) < 0.00001f) return;
        if (area < 0) { (b, c) = (c, b); area = -area; }
        int minX = Math.Max(0, (int)MathF.Floor(MathF.Min(a.X, MathF.Min(b.X, c.X))));
        int maxX = Math.Min(width - 1, (int)MathF.Ceiling(MathF.Max(a.X, MathF.Max(b.X, c.X))));
        int minY = Math.Max(0, (int)MathF.Floor(MathF.Min(a.Y, MathF.Min(b.Y, c.Y))));
        int maxY = Math.Min(height - 1, (int)MathF.Ceiling(MathF.Max(a.Y, MathF.Max(b.Y, c.Y))));
        bool topLeftA = IsTopLeft(b, c), topLeftB = IsTopLeft(c, a), topLeftC = IsTopLeft(a, b);
        for (int y = minY; y <= maxY; y++)
        for (int x = minX; x <= maxX; x++)
        {
            float px = x + 0.5f, py = y + 0.5f;
            float ea = Edge(b, c, px, py), eb = Edge(c, a, px, py), ec = Edge(a, b, px, py);
            // The top-left rule gives shared triangle edges exactly one owner.
            if (!Inside(ea, topLeftA) || !Inside(eb, topLeftB) || !Inside(ec, topLeftC)) continue;
            float wa = ea / area, wb = eb / area, wc = ec / area;
            float inverse = wa * a.InverseDepth + wb * b.InverseDepth + wc * c.InverseDepth;
            if (!(inverse > 0)) continue;
            float depth = 1.0f / inverse;
            int index = y * width + x;
            if (depth > depths[index] + 0.00001f) continue;
            int color = solidColor;
            if (texture != null)
            {
                float u = (wa * a.UOverDepth + wb * b.UOverDepth + wc * c.UOverDepth) / inverse;
                float v = (wa * a.VOverDepth + wb * b.VOverDepth + wc * c.VOverDepth) / inverse;
                color = texture.Sample(u, v);
                if ((uint)color >> 24 == 0 || opacity <= 0) continue;
                color = Blend(colors[index], color, opacity);
            }
            colors[index] = color;
            depths[index] = depth;
        }
    }

    private void DrawLine(PointF start, float startDepth, PointF end, float endDepth,
        int color, float lineWidth)
    {
        float ax = start.X * SampleScale, ay = start.Y * SampleScale;
        float bx = end.X * SampleScale, by = end.Y * SampleScale;
        float dx = bx - ax, dy = by - ay, lengthSquared = dx * dx + dy * dy;
        if (!float.IsFinite(lengthSquared) || lengthSquared < 0.00001f) return;
        float radius = lineWidth * SampleScale * 0.5f;
        int minX = Math.Max(0, (int)MathF.Floor(MathF.Min(ax, bx) - radius));
        int maxX = Math.Min(width - 1, (int)MathF.Ceiling(MathF.Max(ax, bx) + radius));
        int minY = Math.Max(0, (int)MathF.Floor(MathF.Min(ay, by) - radius));
        int maxY = Math.Min(height - 1, (int)MathF.Ceiling(MathF.Max(ay, by) + radius));
        for (int y = minY; y <= maxY; y++)
        for (int x = minX; x <= maxX; x++)
        {
            float px = x + 0.5f, py = y + 0.5f;
            float t = Math.Clamp(((px - ax) * dx + (py - ay) * dy) / lengthSquared, 0, 1);
            float cx = ax + t * dx, cy = ay + t * dy;
            float ox = px - cx, oy = py - cy;
            if (ox * ox + oy * oy > radius * radius) continue;
            float inverse = (1 - t) / startDepth + t / endDepth;
            if (!(inverse > 0)) continue;
            float edgeDepth = 1.0f / inverse;
            int centerX = Math.Clamp((int)MathF.Floor(cx), 0, width - 1);
            int centerY = Math.Clamp((int)MathF.Floor(cy), 0, height - 1);
            int index = y * width + x;
            if (edgeDepth > depths[centerY * width + centerX] + EdgeDepthBias ||
                edgeDepth > depths[index] + EdgeDepthBias) continue;
            colors[index] = color;
        }
    }

    private static float Edge(RasterVertex a, RasterVertex b, float x, float y) =>
        (b.X - a.X) * (y - a.Y) - (b.Y - a.Y) * (x - a.X);
    private static bool IsTopLeft(RasterVertex a, RasterVertex b) =>
        a.Y > b.Y || (a.Y == b.Y && a.X < b.X);
    private static bool Inside(float edge, bool topLeft) => edge > 0 || (edge == 0 && topLeft);

    private static int Blend(int background, int foreground, float opacity)
    {
        int alpha = (int)MathF.Round(((uint)foreground >> 24) * opacity);
        int backgroundAlpha = (int)((uint)background >> 24);
        int remaining = backgroundAlpha * (255 - alpha) / 255;
        int outputAlpha = alpha + remaining;
        if (outputAlpha == 0) return 0;
        int r = (((foreground >> 16) & 255) * alpha + ((background >> 16) & 255) * remaining) / outputAlpha;
        int g = (((foreground >> 8) & 255) * alpha + ((background >> 8) & 255) * remaining) / outputAlpha;
        int b = ((foreground & 255) * alpha + (background & 255) * remaining) / outputAlpha;
        return (outputAlpha << 24) | (r << 16) | (g << 8) | b;
    }

    private readonly record struct RasterVertex(float X, float Y, float InverseDepth,
        float UOverDepth, float VOverDepth);
}

// Cache photo pixels once; per-frame sampling does not call Bitmap.GetPixel.
internal sealed class ReelSceneTexture
{
    public int Width { get; }
    public int Height { get; }
    private readonly int[] pixels;

    public ReelSceneTexture(Image image)
    {
        Width = image.Width;
        Height = image.Height;
        pixels = new int[Width * Height];
        using Bitmap bitmap = new(Width, Height, PixelFormat.Format32bppArgb);
        using (Graphics graphics = Graphics.FromImage(bitmap)) graphics.DrawImageUnscaled(image, 0, 0);
        BitmapData data = bitmap.LockBits(new Rectangle(0, 0, Width, Height),
            ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
        try
        {
            for (int y = 0; y < Height; y++)
                Marshal.Copy(IntPtr.Add(data.Scan0, y * data.Stride), pixels, y * Width, Width);
        }
        finally { bitmap.UnlockBits(data); }
    }

    public int Sample(float u, float v)
    {
        int x = Math.Clamp((int)MathF.Floor(u), 0, Width - 1);
        int y = Math.Clamp((int)MathF.Floor(v), 0, Height - 1);
        return pixels[y * Width + x];
    }
}
