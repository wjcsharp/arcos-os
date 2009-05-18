#ifndef __API_H__
#define __API_H__

VOID
Sleep(ULONG Milliseconds);

ULONG
GetProcessId();

ULONG
GetProcessPriority();

VOID
KillMe();

ULONG
CreateProcess(
        PCHAR ProgramName,
        ULONG Priority,
        PHANDLE ProcessHandle,
        PCHAR Args
        );


//put your own function names here
//CreateProcess(...)

//CreateFile(...)


#endif
