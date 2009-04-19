/*

Module Name:

    halp.h

Abstract:

    Win32 HAL private include file.

Author:

    Michal Strehovsky

Revision History:

*/

#ifndef __HALP_H__
#define __HALP_H__

#include <win32/minwin.h>

extern HANDLE hConsoleIn;
extern HANDLE hConsoleOut;

extern HANDLE hInterruptMutex;
extern HANDLE hOsThread;
extern HANDLE hInterruptProcessedEvent;
extern PVOID exceptionStack;

#define EXCEPTION_STACK_SIZE    16384


typedef struct _EXCEPTION_FRAME {
    CONTEXT Context;
    ULONG Reason;
} EXCEPTION_FRAME, *PEXCEPTION_FRAME;

VOID __fastcall
HalHandleException(PEXCEPTION_FRAME exceptionFrame);

DWORD WINAPI
HalReaderThread(PVOID lpParameter);

DWORD WINAPI 
HalTimerThread(PVOID lpParameter);

#endif

