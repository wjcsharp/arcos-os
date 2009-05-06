/* 
 * File:   ps.h
 * Author: Magnus Soderling
 *
 * Created on den 4 maj 2009, 22:44
 */
#ifndef _PS_H
#define	_PS_H

#include <arcos.h>
#include <mm.h>
//#include <io.h> Not implemented

STATUS
PsCreateProcess(
        PVOID (*PFunction)(),
        ULONG Priority,
        PHANDLE ProcessHandle,
        PCHAR Args
        );

STATUS
PsKillProcess(
        HANDLE ProcessHandle
        );

STATUS
PsProcessExitStatus(
        HANDLE ProcessHandle,
        PSTATUS ExitStatus
        );

#endif	/* _PS_H */

