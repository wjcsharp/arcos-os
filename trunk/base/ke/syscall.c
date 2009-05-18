#include <arcos.h>
#include <ke.h>
#include <ps.h>

VOID
KeSetSyscallResult(
        PPROCESS Process,
        ULONG Result
        ) {
    ASSERT(Process);

#ifdef MIPSEL32
    Process->Context.V0 = Result;
#else
#error Not implemented
#endif
}

VOID
KeSystemService(
        ULONG ServiceNumber,
        ULONG Arg0,
        ULONG Arg1,
        ULONG Arg2,
        ULONG Arg3
        ) {
    STATUS status;
    switch (ServiceNumber) {
        case 0:
            KeSuspendProcess(Arg0);
            break;

        case 1:
            KeSetSyscallResult(KeCurrentProcess, KeCurrentProcess->PID);
            break;

        case 2:
            PsKillMe();
            break;

            case 3:
            status = PsCreateProcessByName((PCHAR) Arg0, Arg1,(PHANDLE) Arg2, (PCHAR) Arg3);
            KeSetSyscallResult(KeCurrentProcess, status);
            break;



        default:
            // probably a bugcheck is overreacting but what the hell
            KeBugCheck("Bad syscall. I think I will just die now.");
    }
}
