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
// Defines size of memory to be allocated for a process
//
#define PROCESS_MEMORY_SIZE 32768

VOID
PsInitialize();
//Setting up the process manager.

STATUS
PsCreateProcess(
        PVOID (*PStartingAddress)(),
        ULONG Priority,
        PHANDLE ProcessHandle,
        PCHAR Args
        );

STATUS
PsKillProcess(
        PPROCESS Process
        );

STATUS
PsGetExitStatus(
        HANDLE ProcessHandle,
        PULONG ExitCode
        );

STATUS
PsGetPriority(
	HANDLE  ProcessHandle,
	PULONG Priority
	);

STATUS
PsGetState(
	HANDLE  ProcessHandle,
	PULONG State
	);

STATUS
PsOpenProcess(
	ULONG PID,
	PHANDLE ProcessHandle
	);
STATUS
PsGetRunningProcesses(
	PPROCESS Buffer[],
	ULONG Buffsize,
	PULONG NumberRunningProcesses
	);

#endif	/* _PS_H */

