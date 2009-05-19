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

HANDLE
CreateFile(ULONG filename);	// 's' for serial, 'l' for lcd. Serial is used for input also.

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

STATUS
ChangePriority(
        ULONG PID,
        ULONG NewPrio);

#endif
