#include <arcos.h>
#include <kd.h>
#include <rtl.h>
#include <api.h>
#include <apps.h>

VOID
AppKill() {
    ULONG PID, argLength;
    STATUS status;
    CHAR Args[25];
    HANDLE outp;
    Sleep(5000);
    outp = CreateFile('s');
    //Get function args
    argLength = CopyArgs(Args, 25);
    //KdPrint(":::%s:::%d", Args, argLength);
    if (argLength == 0) {
        WriteString(outp, "\n\rKill needs an argument e.g. 'kill 5'\r\n");
        //KdPrint("Kill needs an argument e.g. 'kill 5'"); //BUGBUGBUG
        Sleep(5000);
        CloseHandle(outp);
        KillMe();
    }

    PID = RtlAtoUL(Args);

    status = KillByPID(PID, 1); //ExitStatus 1 == murdered
    if (0 == status)
        KdPrint("Killed %d", PID); //BUGBUGBUG
    else
        KdPrint("Failed to kill %d", PID); //BUGBUGBUG
    CloseHandle(outp);
    KillMe();
}
