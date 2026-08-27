#include "DFProtocol.h"

#include <string.h>

const char DF_Protocol_MainToRod_PowerOn[] = "05";
const char DF_Protocol_MainToRod_RodInfoRequest[] = "07";
const char DF_Protocol_MainToRod_Alive[] = "09";
const char DF_Protocol_MainToRod_Break[] = "11";
const char DF_Protocol_MainToRod_ImuDataControl[] = "13";
const char DF_Protocol_MainToRod_VerticalMotor[] = "19";
const char DF_Protocol_MainToRod_VersionRead[] = "21";
const char DF_Protocol_MainToRod_ButtonLed[] = "27";
const char DF_Protocol_MainToRod_AllOutputsOff[] = "29";
const char DF_Protocol_MainToRod_MainAddress[] = "31";
const char DF_Protocol_MainToRod_ApInfo[] = "33";
const char DF_Protocol_MainToRod_Sleep[] = "34";

const char DF_Protocol_RodToMain_BoardType[] = "02";
const char DF_Protocol_RodToMain_Alive[] = "10";
const char DF_Protocol_RodToMain_ImuData[] = "14";
const char DF_Protocol_RodToMain_Button[] = "16";
const char DF_Protocol_RodToMain_Encoder[] = "18";
const char DF_Protocol_RodToMain_Version[] = "22";
const char DF_Protocol_RodToMain_ImuConnection[] = "24";
const char DF_Protocol_RodToMain_Battery[] = "26";
const char DF_Protocol_RodToMain_RodAddress[] = "32";
const char DF_Protocol_RodToMain_Sleep[] = "34";

int DF_Protocol_Encode(char* output, size_t outputCapacity, const char* pidText, const char* payload, size_t payloadLength)
{
    size_t wireLength;

    if (output == 0 || pidText == 0 || payload == 0)
    {
        return -1;
    }
    if (pidText[0] < '0' || pidText[0] > '9' || pidText[1] < '0' || pidText[1] > '9' || pidText[2] != '\0')
    {
        return -2;
    }

    wireLength = DF_Protocol_PidTextLength + payloadLength;
    if (wireLength > DF_Protocol_MaxWireLength || outputCapacity <= wireLength)
    {
        return -3;
    }

    output[0] = pidText[0];
    output[1] = pidText[1];
    if (payloadLength > 0)
    {
        memcpy(output + DF_Protocol_PidTextLength, payload, payloadLength);
    }
    output[wireLength] = '\0';
    return (int)wireLength;
}

int DF_Protocol_Decode(const char* input, size_t inputLength, DF_Protocol_MessageView* message)
{
    if (input == 0 || message == 0)
    {
        return -1;
    }
    if (inputLength < DF_Protocol_PidTextLength || inputLength > DF_Protocol_MaxWireLength)
    {
        return -2;
    }
    if (input[0] < '0' || input[0] > '9' || input[1] < '0' || input[1] > '9')
    {
        return -3;
    }

    message->pid = ((int)(input[0] - '0') * 10) + (int)(input[1] - '0');
    message->payload = input + DF_Protocol_PidTextLength;
    message->payloadLength = inputLength - DF_Protocol_PidTextLength;
    return 0;
}
