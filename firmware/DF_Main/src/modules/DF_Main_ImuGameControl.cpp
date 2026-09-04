// Main-side IMU calibration and game steering conversion.
#include "../DF_Main_Internal.h"
#include "DF_Main_ImuGameControl.h"
#include <math.h>

enum
{
	DF_MAIN_IMU_GAME_IDLE = 0,
	DF_MAIN_IMU_GAME_WAIT_STILL,
	DF_MAIN_IMU_GAME_SAMPLE,
	DF_MAIN_IMU_GAME_READY
};

static const float DF_MAIN_IMU_GAME_STILL_GYRO_LIMIT = 3.0f;
static const unsigned long DF_MAIN_IMU_GAME_STILL_TIME_MS = 3000UL;
static const unsigned long DF_MAIN_IMU_GAME_SAMPLE_TIME_MS = 2000UL;
static const float DF_MAIN_IMU_GAME_YAW_DEAD_ZONE = 2.0f;
static const float DF_MAIN_IMU_GAME_YAW_SENSITIVITY = 0.65f;
static const float DF_MAIN_IMU_GAME_VIRTUAL_ROLL_LIMIT = 30.0f;
static const float DF_MAIN_IMU_GAME_ROLL_LIMIT = 45.0f;
static const float DF_MAIN_IMU_GAME_RETURN_TIME_SEC = 0.9f;

static int DF_Main_ImuGame_Mode = DF_MAIN_IMU_GAME_IDLE;
static int DF_Main_ImuGame_Charging = 0;
static int DF_Main_ImuGame_HasCalibration = 0;
static int DF_Main_ImuGame_StartedImu = 0;
static unsigned long DF_Main_ImuGame_LastTick = 0;
static unsigned long DF_Main_ImuGame_StableTime = 0;
static unsigned long DF_Main_ImuGame_SampleTime = 0;
static unsigned long DF_Main_ImuGame_SampleCount = 0;
static float DF_Main_ImuGame_GyroXSum = 0.0f;
static float DF_Main_ImuGame_GyroYSum = 0.0f;
static float DF_Main_ImuGame_GyroZSum = 0.0f;
static float DF_Main_ImuGame_GyroXBias = 0.0f;
static float DF_Main_ImuGame_GyroYBias = 0.0f;
static float DF_Main_ImuGame_GyroZBias = 0.0f;
static float DF_Main_ImuGame_VirtualRoll = 0.0f;
static float DF_Main_ImuGame_GameRoll = 0.0f;

static float DF_Main_ImuGame_Limit(float value, float minimum, float maximum)
{
	if(value < minimum) return minimum;
	if(value > maximum) return maximum;
	return value;
}

static void DF_Main_ImuGame_ResetSamples()
{
	DF_Main_ImuGame_SampleTime = 0;
	DF_Main_ImuGame_SampleCount = 0;
	DF_Main_ImuGame_GyroXSum = 0.0f;
	DF_Main_ImuGame_GyroYSum = 0.0f;
	DF_Main_ImuGame_GyroZSum = 0.0f;
}

static int DF_Main_ImuGame_ParsePayload(const String &payload, float values[6])
{
	int start = 0;
	int i;
	for(i = 0; i < 6; i++)
	{
		int comma = payload.indexOf(',', start);
		if((i < 5) && (comma < 0)) return 0;
		String token = (comma < 0) ? payload.substring(start) : payload.substring(start, comma);
		if(token.length() == 0) return 0;
		values[i] = token.toFloat();
		start = comma + 1;
	}
	return 1;
}

static void DF_Main_ImuGame_StopTemporaryImu()
{
	if(DF_Main_ImuGame_StartedImu)
	{
		SetIMU_Measure_Out_OnOff(IMU_DATA_OFF);
		DF_Main_ImuGame_StartedImu = 0;
	}
}

void DF_Main_ImuGame_SetCharging(int charging)
{
	charging = charging ? 1 : 0;
	if(charging == DF_Main_ImuGame_Charging) return;
	DF_Main_ImuGame_Charging = charging;
	DF_Main_ImuGame_VirtualRoll = 0.0f;
	DF_Main_ImuGame_LastTick = millis();

	if(charging)
	{
		DF_Main_ImuGame_Mode = DF_MAIN_IMU_GAME_WAIT_STILL;
		DF_Main_ImuGame_StableTime = 0;
		DF_Main_ImuGame_ResetSamples();
		if((0 == reqImuMeasFlag) && (ROD_CONN == rod_conn_status) && (CONNECT == imu_conn_status))
		{
			SetIMU_Measure_Out_OnOff(IMU_DATA_ON);
			DF_Main_ImuGame_StartedImu = 1;
		}
		LogPrintln(" LG] IMUGAME charge calibration wait");
	}
	else
	{
		DF_Main_ImuGame_Mode = DF_Main_ImuGame_HasCalibration ? DF_MAIN_IMU_GAME_READY : DF_MAIN_IMU_GAME_IDLE;
		DF_Main_ImuGame_StopTemporaryImu();
		LogPrintln(" LG] IMUGAME charge calibration exit");
	}
}

static void DF_Main_ImuGame_Update(const float values[6])
{
	unsigned long now = millis();
	unsigned long deltaMs = now - DF_Main_ImuGame_LastTick;
	DF_Main_ImuGame_LastTick = now;
	if(deltaMs > 200UL) deltaMs = 0;

	float roll = values[0];
	float gyroX = values[3];
	float gyroY = values[4];
	float gyroZ = values[5];
	float gyroMagnitude = sqrtf((gyroX * gyroX) + (gyroY * gyroY) + (gyroZ * gyroZ));
	int stationary = (gyroMagnitude <= DF_MAIN_IMU_GAME_STILL_GYRO_LIMIT) ? 1 : 0;

	if(DF_Main_ImuGame_Charging)
	{
		DF_Main_ImuGame_VirtualRoll = 0.0f;
		DF_Main_ImuGame_GameRoll = roll;
		if(DF_MAIN_IMU_GAME_WAIT_STILL == DF_Main_ImuGame_Mode)
		{
			if(stationary) DF_Main_ImuGame_StableTime += deltaMs;
			else DF_Main_ImuGame_StableTime = 0;
			if(DF_Main_ImuGame_StableTime >= DF_MAIN_IMU_GAME_STILL_TIME_MS)
			{
				DF_Main_ImuGame_Mode = DF_MAIN_IMU_GAME_SAMPLE;
				DF_Main_ImuGame_ResetSamples();
				LogPrintln(" LG] IMUGAME gyro bias sampling");
			}
		}
		else if(DF_MAIN_IMU_GAME_SAMPLE == DF_Main_ImuGame_Mode)
		{
			if(!stationary)
			{
				DF_Main_ImuGame_Mode = DF_MAIN_IMU_GAME_WAIT_STILL;
				DF_Main_ImuGame_StableTime = 0;
				DF_Main_ImuGame_ResetSamples();
			}
			else
			{
				DF_Main_ImuGame_GyroXSum += gyroX;
				DF_Main_ImuGame_GyroYSum += gyroY;
				DF_Main_ImuGame_GyroZSum += gyroZ;
				DF_Main_ImuGame_SampleCount++;
				DF_Main_ImuGame_SampleTime += deltaMs;
				if((DF_Main_ImuGame_SampleTime >= DF_MAIN_IMU_GAME_SAMPLE_TIME_MS) && (DF_Main_ImuGame_SampleCount > 0))
				{
					DF_Main_ImuGame_GyroXBias = DF_Main_ImuGame_GyroXSum / DF_Main_ImuGame_SampleCount;
					DF_Main_ImuGame_GyroYBias = DF_Main_ImuGame_GyroYSum / DF_Main_ImuGame_SampleCount;
					DF_Main_ImuGame_GyroZBias = DF_Main_ImuGame_GyroZSum / DF_Main_ImuGame_SampleCount;
					DF_Main_ImuGame_HasCalibration = 1;
					DF_Main_ImuGame_Mode = DF_MAIN_IMU_GAME_READY;
					DF_Main_ImuGame_StopTemporaryImu();
					LogPrintln(" LG] IMUGAME gyro bias calibration complete");
				}
			}
		}
		return;
	}

	if(!DF_Main_ImuGame_HasCalibration)
	{
		DF_Main_ImuGame_VirtualRoll = 0.0f;
		DF_Main_ImuGame_GameRoll = roll;
		return;
	}

	float deltaSeconds = ((float)deltaMs) / 1000.0f;
	float yawRate = gyroZ - DF_Main_ImuGame_GyroZBias;
	if(fabsf(yawRate) >= DF_MAIN_IMU_GAME_YAW_DEAD_ZONE)
	{
		DF_Main_ImuGame_VirtualRoll -= yawRate * deltaSeconds * DF_MAIN_IMU_GAME_YAW_SENSITIVITY;
	}
	else if(deltaSeconds > 0.0f)
	{
		DF_Main_ImuGame_VirtualRoll *= expf(-deltaSeconds / DF_MAIN_IMU_GAME_RETURN_TIME_SEC);
		if(fabsf(DF_Main_ImuGame_VirtualRoll) < 0.05f) DF_Main_ImuGame_VirtualRoll = 0.0f;
	}

	DF_Main_ImuGame_VirtualRoll = DF_Main_ImuGame_Limit(DF_Main_ImuGame_VirtualRoll,
		-DF_MAIN_IMU_GAME_VIRTUAL_ROLL_LIMIT, DF_MAIN_IMU_GAME_VIRTUAL_ROLL_LIMIT);
	DF_Main_ImuGame_GameRoll = DF_Main_ImuGame_Limit(roll + DF_Main_ImuGame_VirtualRoll,
		-DF_MAIN_IMU_GAME_ROLL_LIMIT, DF_MAIN_IMU_GAME_ROLL_LIMIT);
}

String DF_Main_ImuGame_ProcessPayload(const String &payload, int gameOutput)
{
	float values[6];
	if(!DF_Main_ImuGame_ParsePayload(payload, values)) return payload;
	DF_Main_ImuGame_Update(values);
	if(!gameOutput || !DF_Main_ImuGame_HasCalibration || DF_Main_ImuGame_Charging) return payload;

	int firstComma = payload.indexOf(',');
	if(firstComma < 0) return payload;
	return String(DF_Main_ImuGame_GameRoll, 2) + payload.substring(firstComma);
}

