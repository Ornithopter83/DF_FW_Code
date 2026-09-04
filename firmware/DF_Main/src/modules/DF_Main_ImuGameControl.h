#pragma once

#include <Arduino.h>

void DF_Main_ImuGame_SetCharging(int charging);
String DF_Main_ImuGame_ProcessPayload(const String &payload, int gameOutput);

