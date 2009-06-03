//
// File:   kill.c
// Author: Magnus Söderling
//
//


#include <arcos.h>
#include <kd.h>
#include <rtl.h>
#include <api.h>
#include <apps.h>

VOID
AppKill() {
    ULONG PID, argLength;
    STATUS status;
    CHAR args[25], outstring[80];
    HANDLE outp;

    outp = CreateFile('s');
    //Get function args
    argLength = CopyArgs(args, 25);

    if (argLength == 0) {
        WriteString(outp, "\n\rKill needs an argument e.g. 'kill 5'\r\n");
        CloseHandle(outp);
        KillMe();
    }

    PID = RtlAtoUL(args);

    status = KillByPID(PID, 1); //ExitStatus 1 == murdered
    if (0 == status) {
        RtlFormatString(outstring,80,"\r\n Killed %d\r\n", PID);
        WriteString(outp, outstring);
    } else{
     RtlFormatString(outstring,80,"\r\n Failed to kill %d\r\n", PID);
        WriteString(outp, outstring);
    }
    CloseHandle(outp);
    KillMe();
}
