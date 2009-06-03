#include <arcos.h>
#include <apps.h>
#include <api.h>
#include <rtl.h>
#include <kd.h>

#define DRONE_SLEEP_TIME 5000

VOID
AppDrone() {
    ULONG i;
    HANDLE outp;
    CHAR outstring[80];

    outp = CreateFile('s');

    RtlFormatString(outstring, 80, "\r\nDrone with PID:%d started\r\n", GetProcessId());
    WriteString(outp, outstring);
    KdPrint("in drone 1");
    Sleep(DRONE_SLEEP_TIME);

    RtlFormatString(outstring, 80, "\r\nDrone with PID:%d still alive\r\n", GetProcessId());

    for (i = 0; i < 3; i++) {
        KdPrint("in drone 2");
        WriteString(outp, outstring);
        Sleep(DRONE_SLEEP_TIME);
    }
    RtlFormatString(outstring, 80, "\r\nDrone with PID:%d killing my self\r\n", GetProcessId());
    KdPrint("in drone 3");
    WriteString(outp, outstring);
    CloseHandle(outp);
    KillMe();
}
