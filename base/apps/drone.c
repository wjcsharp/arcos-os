#include <arcos.h>
#include <apps.h>
#include <api.h>
#include <rtl.h>

VOID
AppDrone() {
    ULONG i;
    HANDLE outp;
    CHAR outstring[80];

    outp = CreateFile('s');

    RtlFormatString(outstring, 80, "\r\nDrone with PID:%d started\r\n", GetProcessId());
    WriteString(outp, outstring);
    Sleep(15000);

    RtlFormatString(outstring, 80, "\r\nDrone with PID:%d still alive\r\n", GetProcessId());

    for (i = 0; i < 3; i++) {
        WriteString(outp, outstring);
        Sleep(15000);
    }
    RtlFormatString(outstring, 80, "\r\nDrone with PID:%d killing my self\r\n", GetProcessId());
    WriteString(outp, outstring);
    CloseHandle(outp);
    KillMe();
}
