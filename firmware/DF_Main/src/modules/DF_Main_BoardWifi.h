#pragma once

#include <Arduino.h>

void DF_Main_BoardWifi_Begin();
void DF_Main_BoardWifi_Poll();
bool DF_Main_BoardWifi_HandlePairCommand(const String &message);
void DF_Main_BoardWifi_BeginShutdown();
void DF_Main_BoardWifi_RetryShutdown();
void DF_Main_BoardWifi_CancelShutdown();
