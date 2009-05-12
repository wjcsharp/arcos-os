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

//
// Defines size of memory to be allocated for a process
//
#define PROCESS_MEMORY_SIZE 32768

STATUS
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
PsGetProcessExitStatus(
        HANDLE ProcessHandle,
        PULONG ExitCode
        );

#endif	/* _PS_H */

