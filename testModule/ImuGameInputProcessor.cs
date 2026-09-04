using System.Diagnostics;

namespace DFTestModule;

internal sealed class ImuGameInputProcessor
{
    private const float StillGyroLimit = 3.0f;
    private const double StillRequiredSeconds = 3.0;
    private const double SampleRequiredSeconds = 2.0;
    private const float YawDeadZone = 2.0f;
    private const float YawSensitivity = 0.65f;
    private const float VirtualRollLimit = 30.0f;
    private const float GameRollLimit = 45.0f;
    private const float ReturnTimeSeconds = 0.9f;

    private CalibrationMode mode;
    private long lastTimestamp;
    private double stableSeconds;
    private double sampleSeconds;
    private int sampleCount;
    private double gyroXSum;
    private double gyroYSum;
    private double gyroZSum;
    private float gyroXBias;
    private float gyroYBias;
    private float gyroZBias;

    public string StatusText { get; private set; } = "보정 전";
    public bool HasCalibration { get; private set; }
    public float CorrectedRoll { get; private set; }
    public float CorrectedPitch { get; private set; }
    public float VirtualRoll { get; private set; }
    public float GameRoll { get; private set; }

    public void StartCalibration()
    {
        mode = CalibrationMode.WaitingForStillness;
        HasCalibration = false;
        VirtualRoll = 0;
        GameRoll = 0;
        stableSeconds = 0;
        ResetSamples();
        lastTimestamp = Stopwatch.GetTimestamp();
        StatusText = "정지 확인 0.0/3.0초";
    }

    public void CancelCalibration()
    {
        if (mode == CalibrationMode.WaitingForStillness || mode == CalibrationMode.Sampling)
        {
            mode = CalibrationMode.Idle;
            StatusText = "보정 취소";
        }
        lastTimestamp = 0;
    }

    public void Reset()
    {
        mode = CalibrationMode.Idle;
        HasCalibration = false;
        CorrectedRoll = 0;
        CorrectedPitch = 0;
        VirtualRoll = 0;
        GameRoll = 0;
        lastTimestamp = 0;
        StatusText = "보정 전";
        ResetSamples();
    }

    public void Update(float roll, float pitch, float gyroX, float gyroY, float gyroZ)
    {
        long now = Stopwatch.GetTimestamp();
        double deltaSeconds = lastTimestamp == 0 ? 0 : (double)(now - lastTimestamp) / Stopwatch.Frequency;
        lastTimestamp = now;
        if (deltaSeconds < 0 || deltaSeconds > 0.2) deltaSeconds = 0;

        float gyroMagnitude = MathF.Sqrt(gyroX * gyroX + gyroY * gyroY + gyroZ * gyroZ);
        bool stationary = gyroMagnitude <= StillGyroLimit;

        if (mode == CalibrationMode.WaitingForStillness)
        {
            stableSeconds = stationary ? stableSeconds + deltaSeconds : 0;
            StatusText = string.Format(System.Globalization.CultureInfo.InvariantCulture,
                "정지 확인 {0:0.0}/3.0초", Math.Min(stableSeconds, StillRequiredSeconds));
            if (stableSeconds >= StillRequiredSeconds)
            {
                mode = CalibrationMode.Sampling;
                ResetSamples();
                StatusText = "보정값 수집 0.0/2.0초";
            }
            return;
        }

        if (mode == CalibrationMode.Sampling)
        {
            if (!stationary)
            {
                mode = CalibrationMode.WaitingForStillness;
                stableSeconds = 0;
                ResetSamples();
                StatusText = "움직임 감지 - 다시 대기";
                return;
            }

            AddSample(gyroX, gyroY, gyroZ);
            sampleSeconds += deltaSeconds;
            StatusText = string.Format(System.Globalization.CultureInfo.InvariantCulture,
                "보정값 수집 {0:0.0}/2.0초", Math.Min(sampleSeconds, SampleRequiredSeconds));
            if (sampleSeconds >= SampleRequiredSeconds && sampleCount > 0) FinishCalibration();
            return;
        }

        CorrectedRoll = roll;
        CorrectedPitch = pitch;
        if (!HasCalibration)
        {
            VirtualRoll = 0;
            GameRoll = CorrectedRoll;
            return;
        }

        float yawRate = gyroZ - gyroZBias;
        if (MathF.Abs(yawRate) >= YawDeadZone)
        {
            VirtualRoll -= yawRate * (float)deltaSeconds * YawSensitivity;
        }
        else if (deltaSeconds > 0)
        {
            float decay = MathF.Exp(-(float)deltaSeconds / ReturnTimeSeconds);
            VirtualRoll *= decay;
            if (MathF.Abs(VirtualRoll) < 0.05f) VirtualRoll = 0;
        }

        VirtualRoll = Math.Clamp(VirtualRoll, -VirtualRollLimit, VirtualRollLimit);
        GameRoll = Math.Clamp(CorrectedRoll + VirtualRoll, -GameRollLimit, GameRollLimit);
    }

    private void AddSample(float gyroX, float gyroY, float gyroZ)
    {
        gyroXSum += gyroX;
        gyroYSum += gyroY;
        gyroZSum += gyroZ;
        sampleCount++;
    }

    private void FinishCalibration()
    {
        gyroXBias = (float)(gyroXSum / sampleCount);
        gyroYBias = (float)(gyroYSum / sampleCount);
        gyroZBias = (float)(gyroZSum / sampleCount);
        CorrectedRoll = 0;
        CorrectedPitch = 0;
        VirtualRoll = 0;
        GameRoll = 0;
        HasCalibration = true;
        mode = CalibrationMode.Calibrated;
        StatusText = string.Format(System.Globalization.CultureInfo.InvariantCulture,
            "완료 GX{0:0.00} GY{1:0.00} GZ{2:0.00}", gyroXBias, gyroYBias, gyroZBias);
    }

    private void ResetSamples()
    {
        sampleSeconds = 0;
        sampleCount = 0;
        gyroXSum = 0;
        gyroYSum = 0;
        gyroZSum = 0;
    }

    private enum CalibrationMode
    {
        Idle,
        WaitingForStillness,
        Sampling,
        Calibrated
    }
}
