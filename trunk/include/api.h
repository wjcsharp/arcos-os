#ifndef __API_H__
#define __API_H__


#define WriteString(handle,msg) WriteFile(handle, msg, RtlStringLength(msg))

typedef struct _PROCESS_INFO {
    PROCESS_STATE State;
    PCHAR RunningProgram;
    ULONG PID;
    ULONG Priority;
    ULONG CPUTime;
} PROCESS_INFO, *PPROCESS_INFO;

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

HANDLE
CreateFile(ULONG filename); // 's' for serial, 'l' for lcd. Serial is used for input also.

ULONG
WriteFile(
        HANDLE handle,
        PVOID buffer,
        ULONG bufferSize);

ULONG
ReadFile(
        HANDLE handle,
        PVOID buffer,
        ULONG bufferSize);

STATUS
KillByPID(
        ULONG PID,
        ULONG ExitStatus
        );

STATUS
SendMessage(
        ULONG receiverPid,
        ULONG messageType,
        PVOID buffer,
        ULONG bufferSize
        );

PVOID
ReceiveFirst(
        ULONG timeout
        );

ULONG
GetMessageSize(
        PVOID mess
        );

STATUS
CopyMessage(
        PVOID messDest,
        PVOID messSource,
        ULONG bufferSize
        );

STATUS
DeleteMessage(
        PVOID mess
        );

STATUS
ChangePriority(
        ULONG PID,
        ULONG NewPrio
        );

STATUS
GetProcessInfo(
        PPROCESS_INFO Buffer,
        ULONG BufferSize,
        PULONG NumberProcesses
        );

STATUS
SuperviseProc(
        ULONG Supervisor,
        ULONG Pid
        );

PVOID
Malloc(
        ULONG Size
        );

PVOID
Free(
        PVOID BlockBody
        );

ULONG
GetUsedMemSum();

STATUS
WaitForSingleObject(
        HANDLE Handle
        );

//Returns stringlength
ULONG
CopyArgs(
        PCHAR buffer,
        ULONG buffersize
        );

#endif
