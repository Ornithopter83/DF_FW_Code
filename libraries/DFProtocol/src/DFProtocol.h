#pragma once

#include <stddef.h>

enum
{
    DF_Protocol_MaxWireLength = 127,
    DF_Protocol_PidTextLength = 2,
    DF_Protocol_MacAddressTextLength = 17
};

typedef enum
{
    DF_Protocol_Pid_MainToRod_PowerOn = 5,
    DF_Protocol_Pid_MainToRod_RodInfoRequest = 7,
    DF_Protocol_Pid_MainToRod_Alive = 9,
    DF_Protocol_Pid_MainToRod_Break = 11,
    DF_Protocol_Pid_MainToRod_ImuDataControl = 13,
    DF_Protocol_Pid_MainToRod_VerticalMotor = 19,
    DF_Protocol_Pid_MainToRod_VersionRead = 21,
    DF_Protocol_Pid_MainToRod_ButtonLed = 27,
    DF_Protocol_Pid_MainToRod_AllOutputsOff = 29,
    DF_Protocol_Pid_MainToRod_MainAddress = 31,
    DF_Protocol_Pid_MainToRod_ApInfo = 33,
    DF_Protocol_Pid_MainToRod_Sleep = 34
} DF_Protocol_MainToRodPid;

typedef enum
{
    DF_Protocol_Pid_RodToMain_BoardType = 2,
    DF_Protocol_Pid_RodToMain_Alive = 10,
    DF_Protocol_Pid_RodToMain_ImuData = 14,
    DF_Protocol_Pid_RodToMain_Button = 16,
    DF_Protocol_Pid_RodToMain_Encoder = 18,
    DF_Protocol_Pid_RodToMain_Version = 22,
    DF_Protocol_Pid_RodToMain_ImuConnection = 24,
    DF_Protocol_Pid_RodToMain_Battery = 26,
    DF_Protocol_Pid_RodToMain_RodAddress = 32,
    DF_Protocol_Pid_RodToMain_Sleep = 34
} DF_Protocol_RodToMainPid;

extern const char DF_Protocol_MainToRod_PowerOn[];
extern const char DF_Protocol_MainToRod_RodInfoRequest[];
extern const char DF_Protocol_MainToRod_Alive[];
extern const char DF_Protocol_MainToRod_Break[];
extern const char DF_Protocol_MainToRod_ImuDataControl[];
extern const char DF_Protocol_MainToRod_VerticalMotor[];
extern const char DF_Protocol_MainToRod_VersionRead[];
extern const char DF_Protocol_MainToRod_ButtonLed[];
extern const char DF_Protocol_MainToRod_AllOutputsOff[];
extern const char DF_Protocol_MainToRod_MainAddress[];
extern const char DF_Protocol_MainToRod_ApInfo[];
extern const char DF_Protocol_MainToRod_Sleep[];

extern const char DF_Protocol_RodToMain_BoardType[];
extern const char DF_Protocol_RodToMain_Alive[];
extern const char DF_Protocol_RodToMain_ImuData[];
extern const char DF_Protocol_RodToMain_Button[];
extern const char DF_Protocol_RodToMain_Encoder[];
extern const char DF_Protocol_RodToMain_Version[];
extern const char DF_Protocol_RodToMain_ImuConnection[];
extern const char DF_Protocol_RodToMain_Battery[];
extern const char DF_Protocol_RodToMain_RodAddress[];
extern const char DF_Protocol_RodToMain_Sleep[];

typedef struct
{
    int pid;
    const char* payload;
    size_t payloadLength;
} DF_Protocol_MessageView;

int DF_Protocol_Encode(char* output, size_t outputCapacity, const char* pidText, const char* payload, size_t payloadLength);
int DF_Protocol_Decode(const char* input, size_t inputLength, DF_Protocol_MessageView* message);
