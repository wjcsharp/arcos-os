#include <arcos.h>
#include <ke.h>
#include <ps.h>
#include <io.h>
#include <mess.h>

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
            KeSetSyscallResult(KeCurrentProcess, PsCreateProcessByName((PCHAR) Arg0, Arg1, (PHANDLE) Arg2, (PCHAR) Arg3));
            break;

        case 4:
            KeSetSyscallResult(KeCurrentProcess, KeCurrentProcess->Priority);
            break;

        case 5:
            KeSetSyscallResult(KeCurrentProcess, (ULONG)IoCreateFile(Arg0)); // Return HANDLE to file opened.
            break;

        case 6:
            KeSetSyscallResult(KeCurrentProcess, IoWriteFile((HANDLE) Arg0, (PVOID) Arg1, (ULONG) Arg2)); // Return ULONG = nr of bytes written (not implemented though).
            break;

        case 7:
            KeSetSyscallResult(KeCurrentProcess, IoReadFile((HANDLE) Arg0, (PVOID) Arg1, (ULONG) Arg2)); // Return ULONG = nr of bytes read (not implemented).
            break;

        case 8:
            KeSetSyscallResult(KeCurrentProcess, PsKillByPID(Arg0, Arg1));
            break;

        case 10:
            KeSetSyscallResult(KeCurrentProcess, MessSendMessage(Arg0, Arg1, (PVOID) Arg2, Arg3));
            break;

        case 11:
            KeSetSyscallResult(KeCurrentProcess, (ULONG) MessReceiveFirst(Arg0));
            break;

        default:
            // probably a bugcheck is overreacting but what the hell
            KeBugCheck("Bad syscall. I think I will just die now.");
    }
}
