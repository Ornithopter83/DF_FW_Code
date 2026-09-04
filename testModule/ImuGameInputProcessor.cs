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
    private double rollSinSum;
    private double rollCosSum;
    private double pitchSinSum;
    private double pitchCosSum;
    private double gyroXSum;
    private double gyroYSum;
    private double gyroZSum;
    private float rollZero;
    private float pitchZero;
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

            AddSample(roll, pitch, gyroX, gyroY, gyroZ);
            sampleSeconds += deltaSeconds;
            StatusText = string.Format(System.Globalization.CultureInfo.InvariantCulture,
                "보정값 수집 {0:0.0}/2.0초", Math.Min(sampleSeconds, SampleRequiredSeconds));
            if (sampleSeconds >= SampleRequiredSeconds && sampleCount > 0) FinishCalibration();
            return;
        }

        CorrectedRoll = HasCalibration ? Wrap180(roll - rollZero) : roll;
        CorrectedPitch = HasCalibration ? Wrap180(pitch - pitchZero) : pitch;
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

    private void AddSample(float roll, float pitch, float gyroX, float gyroY, float gyroZ)
    {
        double rollRadians = roll * Math.PI / 180.0;
        double pitchRadians = pitch * Math.PI / 180.0;
        rollSinSum += Math.Sin(rollRadians);
        rollCosSum += Math.Cos(rollRadians);
        pitchSinSum += Math.Sin(pitchRadians);
        pitchCosSum += Math.Cos(pitchRadians);
        gyroXSum += gyroX;
        gyroYSum += gyroY;
        gyroZSum += gyroZ;
        sampleCount++;
    }

    private void FinishCalibration()
    {
        rollZero = CircularAverageDegrees(rollSinSum, rollCosSum);
        pitchZero = CircularAverageDegrees(pitchSinSum, pitchCosSum);
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
            "완료 R{0:0.0} P{1:0.0} GZ{2:0.00}", rollZero, pitchZero, gyroZBias);
    }

    private void ResetSamples()
    {
        sampleSeconds = 0;
        sampleCount = 0;
        rollSinSum = 0;
        rollCosSum = 0;
        pitchSinSum = 0;
        pitchCosSum = 0;
        gyroXSum = 0;
        gyroYSum = 0;
        gyroZSum = 0;
    }

    private static float CircularAverageDegrees(double sinSum, double cosSum) =>
        (float)(Math.Atan2(sinSum, cosSum) * 180.0 / Math.PI);

    private static float Wrap180(float value)
    {
        while (value > 180.0f) value -= 360.0f;
        while (value <= -180.0f) value += 360.0f;
        return value;
    }

    private enum CalibrationMode
    {
        Idle,
        WaitingForStillness,
        Sampling,
        Calibrated
    }
}
