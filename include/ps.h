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

//
// Defines size of default memory to be allocated for a process
//
#define PROCESS_MEMORY_SIZE 32768

POBJECT_TYPE processType;

typedef struct _PROCESS_INFO {
    PROCESS_STATE State;
    ULONG RunningProgram;
    ULONG PID;
    ULONG Priority;
    ULONG CPUTime;
} PROCESS_INFO, *PPROCESS_INFO;

typedef struct _PROGRAM {
    PCHAR Name;
    VOID (*Execute)(PCHAR Parameters);
} PROGRAM, *PPROGRAM;

//List of all available PROGRAMS
PROGRAM programs[10];

VOID
MyFirstProgram();

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
PsKillProcess(
        PPROCESS Process,
        ULONG ExitStatus
        );

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

VOID
PsKillMe();

STATUS
PsGetState(
        HANDLE ProcessHandle,
        PULONG State
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
        PPROCESS_INFO Buffer[],
        ULONG Buffsize,
        PULONG NumberProcesses
        );

#endif	/* _PS_H */

