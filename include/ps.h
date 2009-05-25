/* 
 * File:   ps.h
 * Author: Magnus Soderling
 *
 * Created on den 4 maj 2009, 22:44
 */
#ifndef _PS_H
#define	_PS_H

#include <arcos.h>
#include <ps.h>
#include <mm.h>
#include <ob.h>
#include <ke.h>
#include <rtl.h>
#include <api.h>

//
// Defines size of default memory to be allocated for a process
#define PROCESS_MEMORY_SIZE 32768


typedef struct _PROGRAM {
    PCHAR Name;
    VOID(*Execute)(PCHAR Args);
} APPLICATION, PAPPLICATION;



VOID
PsInitialize();
//Setting up the process manager.


STATUS
PsCreateProcess(
        VOID(*PStartingAddress)(),
        ULONG Priority,
        PHANDLE ProcessHandle,
        PCHAR Args
        );


STATUS
PsCreateProcessByName(
        PCHAR ProgramName,
        ULONG Priority,
        PHANDLE ProcessHandle,
        PCHAR Args
        );

STATUS
PsKillProcess(
        PPROCESS PProcess,
        ULONG ExitStatus
        );

STATUS
PsKillByPID(
        ULONG PID,
        ULONG ExitStatus
        );

VOID
PsKillMe();

STATUS
PsGetExitStatus(
        HANDLE ProcessHandle,
        PULONG ExitCode
        );

STATUS
PsGetPriority(
        HANDLE ProcessHandle,
        PULONG Priority
        );

STATUS
PsSupervise(
        ULONG Supervisor,
        ULONG PID
        );

STATUS
PsChangePriority(
        ULONG PID,
        ULONG NewPriority
        );


STATUS
PsGetState(
        HANDLE ProcessHandle,
        PPROCESS_STATE PState
        );

STATUS
PsOpenProcess(
        ULONG PID,
        PHANDLE ProcessHandle
        );

STATUS
PsReferenceProcess(
        ULONG PID,
        PPROCESS *ProcessPtr
        );


STATUS
PsGetProcessesInfo(
        PROCESS_INFO Buffer[],
        ULONG Buffsize,
        PULONG NumberProcesses
        );

STATUS
PsGetPid(
        HANDLE Handle,
        PULONG PPid
        );

#endif	/* _PS_H */
