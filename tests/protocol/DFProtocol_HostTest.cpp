#include "../../libraries/DFProtocol/src/DFProtocol.h"

#include <assert.h>
#include <string.h>

int main()
{
    char buffer[128];
    DF_Protocol_MessageView message;
    int length;

    length = DF_Protocol_Encode(buffer, sizeof(buffer), DF_Protocol_MainToRod_AllOutputsOff, "00", 2);
    assert(length == 4);
    assert(strcmp(buffer, "2900") == 0);

    assert(DF_Protocol_Decode(buffer, (size_t)length, &message) == 0);
    assert(message.pid == DF_Protocol_Pid_MainToRod_AllOutputsOff);
    assert(message.payloadLength == 2);
    assert(DF_Protocol_Decode("0", 1, &message) < 0);
    assert(DF_Protocol_Decode("AA00", 4, &message) < 0);
    return 0;
}
