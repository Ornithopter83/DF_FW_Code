//
//
//

#include "Common.h"
#include "Encoder.h"
#include "SPIFFS.h"
#include <stdlib.h>


static void (*DF_Rod_Encoder_RotateCallback)(String) = NULL;
static void (*DF_Rod_Encoder_CalibrationCallback)(String) = NULL;
static const char DF_Rod_Encoder_CalibrationFile[] = "/encoder_calibration.txt";
static const char DF_Rod_Encoder_CalibrationTempFile[] = "/encoder_calibration.tmp";
static const char DF_Rod_Encoder_CalibrationBackupFile[] = "/encoder_calibration.bak";

Encoder::Encoder()
{
	count = 0;
	calibrationRawPulseCount = 0;
	calibratedPulsesPerRevolution = MAX_COUNT_VX;
	calibrationActive = false;
	calibrationAwaitingApply = false;
	calibrationValid = false;
	calibrationStorageError = false;
	pendingCorrectedOutputCount = 0;
	correctedOutputStepCount = 0;
	lastCLK = 0;
	lastDT = 0;
	currentCLK = 0;
	currentDT = 0;
}

void Encoder::init()
{
	pinMode(ENC_HALL_SEN_PIN, INPUT_PULLUP);
	lastCLK = digitalRead(ENC_HALL_SEN_PIN);
	count = 0;
	calibrationRawPulseCount = 0;
	calibrationActive = false;
	calibrationAwaitingApply = false;
	calibrationStorageError = false;
	pendingCorrectedOutputCount = 0;
	correctedOutputStepCount = 0;
	loadCalibration();
}

//callback Function Name : rotateChangeCallback
void Encoder::setRotateCallback(void (*rotateCallback)(String))
{
  DF_Rod_Encoder_RotateCallback = rotateCallback;
}

void Encoder::setCalibrationCallback(void (*calibrationCallback)(String))
{
	DF_Rod_Encoder_CalibrationCallback = calibrationCallback;
}

void Encoder::beginCalibration()
{
	if (!calibrationActive)
	{
		calibrationRawPulseCount = 0;
		count = 0;
		oldStr = "";
		calibrationActive = true;
		calibrationAwaitingApply = false;
		pendingCorrectedOutputCount = 0;
		correctedOutputStepCount = 0;
	}
	sendCount();
	sendCalibrationStatus('1');
}

void Encoder::completeCalibration()
{
	unsigned long pulseCount = calibrationRawPulseCount;
	if (!calibrationActive || (0 == pulseCount) || (DF_Rod_Encoder_CalibrationMaxPulseCount < pulseCount))
	{
		calibrationActive = false;
		calibrationAwaitingApply = false;
		sendCalibrationStatus('3');
		sendCount();
		return;
	}

	calibrationActive = false;
	calibrationAwaitingApply = true;
	sendCalibrationStatus('7');
	sendCount();
}

void Encoder::applyCalibration()
{
	unsigned long pulseCount = calibrationRawPulseCount;
	if (!calibrationAwaitingApply || (0 == pulseCount) || (DF_Rod_Encoder_CalibrationMaxPulseCount < pulseCount))
	{
		calibrationAwaitingApply = false;
		sendCalibrationStatus('3');
		return;
	}
	if (!saveCalibration(pulseCount))
	{
		calibrationStorageError = true;
		calibrationAwaitingApply = false;
		sendCalibrationStatus('4');
		sendCount();
		return;
	}

	calibratedPulsesPerRevolution = (unsigned int)pulseCount;
	calibrationValid = true;
	calibrationStorageError = false;
	calibrationAwaitingApply = false;
	count = 0;
	pendingCorrectedOutputCount = 0;
	correctedOutputStepCount = 0;
	oldStr = "";
	sendCalibrationStatus('2');
	sendCount();
}

void Encoder::cancelCalibration()
{
	calibrationActive = false;
	calibrationAwaitingApply = false;
	calibrationRawPulseCount = 0;
	count = 0;
	pendingCorrectedOutputCount = 0;
	correctedOutputStepCount = 0;
	oldStr = "";
	sendCalibrationStatus('0');
	sendCount();
}

void Encoder::queryCalibration()
{
	if (calibrationActive)
	{
		sendCalibrationStatus('1');
	}
	else if (calibrationAwaitingApply)
	{
		sendCalibrationStatus('7');
	}
	else if (calibrationStorageError)
	{
		sendCalibrationStatus('4');
	}
	else
	{
		sendCalibrationStatus('0');
	}
}

void Encoder::sendCalibrationStatus(char state)
{
	String payload = String(state);
	payload += ",";
	payload += calibrationValid ? "1" : "0";
	payload += ",";
	payload += String(calibrationRawPulseCount);
	payload += ",";
	payload += String(calibratedPulsesPerRevolution);
	if (DF_Rod_Encoder_CalibrationCallback != NULL)
	{
		DF_Rod_Encoder_CalibrationCallback(payload);
	}
}

bool Encoder::loadCalibration()
{
	File file;
	String value;
	unsigned long pulseCount = 0;
	unsigned int index;
	bool exists = SPIFFS.exists(DF_Rod_Encoder_CalibrationFile);
	calibratedPulsesPerRevolution = MAX_COUNT_VX;
	calibrationValid = false;
	if (!exists)
	{
		if (!SPIFFS.exists(DF_Rod_Encoder_CalibrationBackupFile)) return false;
		if (!SPIFFS.rename(DF_Rod_Encoder_CalibrationBackupFile, DF_Rod_Encoder_CalibrationFile))
		{
			calibrationStorageError = true;
			return false;
		}
		exists = true;
	}

	file = SPIFFS.open(DF_Rod_Encoder_CalibrationFile, "r");
	if (!file || file.isDirectory())
	{
		calibrationStorageError = true;
		return false;
	}
	while (file.available())
	{
		value += (char)file.read();
	}
	file.close();
	value.trim();
	if ((0 == value.length()) || (8 < value.length()))
	{
		calibrationStorageError = true;
		return false;
	}
	for (index = 0; index < value.length(); index++)
	{
		if ((value[index] < '0') || (value[index] > '9'))
		{
			calibrationStorageError = true;
			return false;
		}
		pulseCount = (pulseCount * 10UL) + (unsigned long)(value[index] - '0');
	}
	if ((0 == pulseCount) || (DF_Rod_Encoder_CalibrationMaxPulseCount < pulseCount))
	{
		calibrationStorageError = true;
		return false;
	}
	calibratedPulsesPerRevolution = (unsigned int)pulseCount;
	calibrationValid = true;
	calibrationStorageError = false;
	return true;
}

bool Encoder::saveCalibration(unsigned long pulseCount)
{
	File file;
	String value = String(pulseCount);
	String storedValue;
	unsigned long parsedValue = 0;
	unsigned int index;
	bool hadOriginal = SPIFFS.exists(DF_Rod_Encoder_CalibrationFile);

	if ((0 == pulseCount) || (DF_Rod_Encoder_CalibrationMaxPulseCount < pulseCount)) return false;
	if (SPIFFS.exists(DF_Rod_Encoder_CalibrationTempFile)) SPIFFS.remove(DF_Rod_Encoder_CalibrationTempFile);
	if (SPIFFS.exists(DF_Rod_Encoder_CalibrationBackupFile)) SPIFFS.remove(DF_Rod_Encoder_CalibrationBackupFile);

	file = SPIFFS.open(DF_Rod_Encoder_CalibrationTempFile, "w");
	if (!file || file.isDirectory()) return false;
	if (file.print(value) != value.length())
	{
		file.close();
		SPIFFS.remove(DF_Rod_Encoder_CalibrationTempFile);
		return false;
	}
	file.close();

	file = SPIFFS.open(DF_Rod_Encoder_CalibrationTempFile, "r");
	if (!file || file.isDirectory())
	{
		SPIFFS.remove(DF_Rod_Encoder_CalibrationTempFile);
		return false;
	}
	while (file.available()) storedValue += (char)file.read();
	file.close();
	if (storedValue != value)
	{
		SPIFFS.remove(DF_Rod_Encoder_CalibrationTempFile);
		return false;
	}
	for (index = 0; index < storedValue.length(); index++)
	{
		if ((storedValue[index] < '0') || (storedValue[index] > '9'))
		{
			SPIFFS.remove(DF_Rod_Encoder_CalibrationTempFile);
			return false;
		}
		parsedValue = (parsedValue * 10UL) + (unsigned long)(storedValue[index] - '0');
	}
	if (parsedValue != pulseCount)
	{
		SPIFFS.remove(DF_Rod_Encoder_CalibrationTempFile);
		return false;
	}

	if (hadOriginal && !SPIFFS.rename(DF_Rod_Encoder_CalibrationFile, DF_Rod_Encoder_CalibrationBackupFile))
	{
		SPIFFS.remove(DF_Rod_Encoder_CalibrationTempFile);
		return false;
	}
	if (!SPIFFS.rename(DF_Rod_Encoder_CalibrationTempFile, DF_Rod_Encoder_CalibrationFile))
	{
		if (hadOriginal) SPIFFS.rename(DF_Rod_Encoder_CalibrationBackupFile, DF_Rod_Encoder_CalibrationFile);
		SPIFFS.remove(DF_Rod_Encoder_CalibrationTempFile);
		return false;
	}
	if (SPIFFS.exists(DF_Rod_Encoder_CalibrationBackupFile)) SPIFFS.remove(DF_Rod_Encoder_CalibrationBackupFile);
	return true;
}



//
//
void Encoder::rotate()
{
	unsigned int previousCount;
	unsigned int pulsesPerRevolution;
	unsigned long previousCorrectedStepCount;
	unsigned long currentCorrectedStepCount;
	bool multiplyCorrection;

	currentCLK = digitalRead(ENC_HALL_SEN_PIN);
	multiplyCorrection = calibrationValid && (calibratedPulsesPerRevolution < DF_Rod_Encoder_CalibrationTargetStepCount);
	pulsesPerRevolution = multiplyCorrection ? calibratedPulsesPerRevolution : MAX_COUNT_VX;

	// 2체배, LE and TE ,  6.3 Pulse / Rev =>  12.6 Pulse/Rev (*2) = 25.2 Pulse (Count * 2 ) => 12.6 Pulse/Rev ( Count 1/2)
	if (lastCLK != currentCLK)
	{
		if (calibrationActive)
		{
			calibrationRawPulseCount++;
			if (DF_Rod_Encoder_CalibrationMaxPulseCount < calibrationRawPulseCount)
			{
				calibrationActive = false;
				sendCalibrationStatus('3');
				sendCount();
			}
			else
			{
				sendCalibrationStatus('1');
			}
		}
		else
		{
			previousCount = (unsigned int)count;
			count++;
			if (count > (int)pulsesPerRevolution)
			{
				count = 0;
			}
			if (multiplyCorrection)
			{
				if ((unsigned int)count > previousCount)
				{
					previousCorrectedStepCount = ((unsigned long)previousCount * DF_Rod_Encoder_CalibrationTargetStepCount) / calibratedPulsesPerRevolution;
					currentCorrectedStepCount = ((unsigned long)count * DF_Rod_Encoder_CalibrationTargetStepCount) / calibratedPulsesPerRevolution;
					if (currentCorrectedStepCount > previousCorrectedStepCount)
					{
						pendingCorrectedOutputCount += currentCorrectedStepCount - previousCorrectedStepCount;
					}
				}
			}
			else
			{
				sendCount();
			}
		}
	}
	lastCLK = currentCLK;

	if (!calibrationActive && multiplyCorrection)
	{
		sendPendingCorrectedStep();
	}

}

void Encoder::sendPendingCorrectedStep()
{
	String str;
	if ((0 == pendingCorrectedOutputCount) || (DF_Rod_Encoder_RotateCallback == NULL)) return;
	pendingCorrectedOutputCount--;
	correctedOutputStepCount++;
	if (DF_Rod_Encoder_CalibrationTargetStepCount < correctedOutputStepCount) correctedOutputStepCount = 1;
	str = "+" + fillZero2char((int)correctedOutputStepCount);
	oldStr = str;
	DF_Rod_Encoder_RotateCallback(str);
}


String Encoder::fillZero2char(int _count)
{

  String str;
  if(0) {}
  
  else if (10 > _count)		//  0 + 1char(1~9)
  {
    str = "0" + String(_count);
  }
  else					//  2char (10 ~ 99)
  {
    str = String(_count);
  }
  return str;
}


/*------------------------------------------------------------
  1. VAL = Pulse /2
  2. Send Only Change
------------------------------------------------------------*/
String Encoder::sendCount()
{
	String str = "";
	long correctedStepCount;
	unsigned int pulsesPerRevolution;

	pulsesPerRevolution = (calibrationValid && (calibratedPulsesPerRevolution < DF_Rod_Encoder_CalibrationTargetStepCount))
		? calibratedPulsesPerRevolution : MAX_COUNT_VX;
	if (0 == pulsesPerRevolution) pulsesPerRevolution = MAX_COUNT_VX;
	correctedStepCount = ((long)count * DF_Rod_Encoder_CalibrationTargetStepCount) / (long)pulsesPerRevolution;

	if ( 0 < correctedStepCount)			// +
	{
		str = "+" + fillZero2char(correctedStepCount);
	}
	else if ( 0 > correctedStepCount)		// -
	{
		str = "-" + fillZero2char(abs(correctedStepCount));
	}
	else	// if 0
	{
		str = "000";				// else 000
	}

	// Send MSG, Only Change
	if (!str.equals(oldStr))			// if Change String(CNT)
	{
		oldStr = str;
		//str = "$0700000" + str + "%";

		if (DF_Rod_Encoder_RotateCallback != NULL)
		{
			DF_Rod_Encoder_RotateCallback(str);
		}
	}
	return str;
}


