#include <arcos.h>
#include <kd.h>
#include <rtl.h>
#include <api.h>

VOID
AppKill() {
    ULONG PID, argLength;
    STATUS status;
    CHAR Args[25];
    HANDLE outp;
    //Get function args
    argLength = CopyArgs(Args, 25);
    //KdPrint(":::%s:::%d", Args, argLength);
    if (argLength == 0) {
        KdPrint("Kill needs an argument e.g. 'kill 5'"); //BUGBUGBUG
        KillMe();
    }

    PID = RtlAtoUL(Args);

    status = KillByPID(PID, 1); //ExitStatus 1 == murdered
    if (0 == status)
        KdPrint("Killed %d", PID); //BUGBUGBUG
    else
        KdPrint("Failed to kill %d", PID); //BUGBUGBUG

    KillMe();
}
