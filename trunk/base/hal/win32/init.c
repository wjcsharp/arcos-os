/*

Module Name:

    init.c

Abstract:

    Win32 HAL initialization and high level exception handling code.

Author:

    Michal Strehovsky

Revision History:

*/

#include <types.h>
#include <win32/minwin.h>
#include "halp.h"

PVOID
HalGetFirstUsableMemoryAddress(
    VOID
    )
{
    // dummy
    return 0;
}

VOID __fastcall
HalHandleException(
    PEXCEPTION_FRAME exceptionFrame
    )
{
    static int counter = 0;
    if (++counter % 100 == 0)
        printf("%d\n", counter);
    //if (exceptionFrame->Reason == 0)
    //    HalDisplayString("TICK\n");

    SetEvent(hInterruptProcessedEvent);

    while (1);
}

VOID
HalInitialize(
    VOID
    )
{
    HANDLE hTimerThread, hReaderThread;
    
    hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleMode(hConsoleIn, ENABLE_PROCESSED_INPUT);

    exceptionStack = VirtualAlloc(0, EXCEPTION_STACK_SIZE, 0x1000, 0x04);

    hOsThread = OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId());

    hInterruptMutex = CreateMutex(0, TRUE, 0);

    hInterruptProcessedEvent = CreateEvent(0, FALSE, FALSE, 0);

    hTimerThread = CreateThread(0, 0, HalTimerThread, 0, 0, 0);
    
    hReaderThread = CreateThread(0, 0, HalReaderThread, 0, 0, 0);

    SetThreadPriority(hTimerThread, THREAD_PRIORITY_HIGHEST);
    SetThreadPriority(hReaderThread, THREAD_PRIORITY_HIGHEST);
}

