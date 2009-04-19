/*

Module Name:

    sandbox.c

Abstract:

    Win32 HAL sandbox thread code.

Author:

    Michal Strehovsky

Revision History:

*/

#include <types.h>
#include <win32/minwin.h>
#include "halp.h"

HANDLE hInterruptMutex;
HANDLE hOsThread;
HANDLE hInterruptProcessedEvent;
PVOID exceptionStack;

VOID
DoInterrupt(
    DWORD reason
    )
{
    EXCEPTION_FRAME exceptionFrame;
    CONTEXT handlerContext;

    //
    // acquire interrupt mutex
    //
    WaitForSingleObject(hInterruptMutex, INFINITE);

    //
    // suspend sandbox OS thread
    //
    SuspendThread(hOsThread);

    //
    // save current context information
    //
    exceptionFrame.Context.ContextFlags = CONTEXT_FULL;
    GetThreadContext(hOsThread, &exceptionFrame.Context);

    //
    // set up new context
    //
    handlerContext = exceptionFrame.Context;
    handlerContext.Eip = (ULONG)HalHandleException;
    handlerContext.Ecx = (ULONG)&exceptionFrame;
    handlerContext.Esp = (ULONG)((PCHAR)exceptionStack + EXCEPTION_STACK_SIZE - 32);
    exceptionFrame.Reason = reason;
    

    //
    // resume sandbox
    //
    SetThreadContext(hOsThread, &handlerContext);
    ResumeThread(hOsThread);

    //
    // wait for the machine to process the interrupt
    //
    WaitForSingleObject(hInterruptProcessedEvent, INFINITE);

    //
    // suspend sandbox
    //
    SuspendThread(hOsThread);
    
    // 
    // resume sandbox
    //
    SetThreadContext(hOsThread, &exceptionFrame.Context);
    ResumeThread(hOsThread);
    
    ReleaseMutex(hInterruptMutex);
}

DWORD WINAPI 
HalReaderThread(
  PVOID lpParameter
    )
{
    CHAR buffer;
    DWORD bRead;
    while (1) {
        ReadFile(hConsoleIn, &buffer, 1, &bRead, 0);
        printf("read: %c\n", buffer);
        DoInterrupt(1);
    }
}

DWORD WINAPI 
HalTimerThread(
  PVOID lpParameter
    )
{
    while (1) {
        Sleep(50);
        DoInterrupt(0);
    }
}


