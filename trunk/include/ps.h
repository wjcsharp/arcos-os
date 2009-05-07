/* 
 * File:   ps.h
 * Author: Magnus Soderling
 *
 * Created on den 4 maj 2009, 22:44
 */
#ifndef _PS_H
#define	_PS_H

#include <arcos.h>
//#include <io.h> Not implemented

//
// Defines size of memory to be allocated for a process
//
#define PROCESS_MEMORY_TO_ALLOCATE 65536

STATUS
PsCreateProcess(
        PVOID (*PStartingAddress)(),
        ULONG Priority,
        PHANDLE ProcessHandle,
        PCHAR Args
        );

STATUS
PsKillProcess(
        HANDLE ProcessHandle
        );

STATUS
PsGetProcessExitStatus(
        HANDLE ProcessHandle,
        PULONG ExitCode
        );

#endif	/* _PS_H */

