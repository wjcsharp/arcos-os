#ifndef __MINWIN_H__
#define __MINWIN_H__

#include <arcos.h>

#define WINAPI  __stdcall
#define DWORD   ULONG
#define PDWORD  PULONG
#define WORD    USHORT

BOOL WINAPI
WriteFile(HANDLE hFile, PVOID lpBuffer, DWORD nNumberOfBytesToWrite, 
    PDWORD lpNumberOfBytesWritten, PVOID lpOverlapped);

BOOL WINAPI
ReadFile(HANDLE hFile, PVOID lpBuffer, DWORD nNumberOfBytesToRead, 
    PDWORD lpNumberOfBytesRead, PVOID lpOverlapped);

HANDLE WINAPI
GetStdHandle(DWORD nStdHandle);

#define STD_INPUT_HANDLE    ((DWORD)-10)
#define STD_OUTPUT_HANDLE   ((DWORD)-11)
#define STD_ERROR_HANDLE    ((DWORD)-12)

BOOL WINAPI
SetConsoleTextAttribute(HANDLE hConsoleOutput, WORD wAttributes);

BOOL WINAPI 
SetConsoleMode(HANDLE hConsoleHandle, DWORD dwMode);

#define ENABLE_PROCESSED_INPUT 0x0001

PVOID WINAPI
VirtualAlloc(PVOID lpAddress, DWORD dwSize, DWORD flAllocationType, DWORD flProtect);

HANDLE WINAPI
CreateThread(PVOID lpThreadAttributes, DWORD dwStackSize, PVOID lpStartAddress, 
    PVOID lpParameter, DWORD dwCreationFlags, PDWORD lpThreadId);

DWORD WINAPI
ResumeThread(HANDLE hThread);

DWORD WINAPI
SuspendThread(HANDLE hThread);

#define THREAD_PRIORITY_HIGHEST 2

BOOL WINAPI
SetThreadPriority(HANDLE hThread, int nPriority);

VOID WINAPI
Sleep(DWORD dwMilliseconds);

HANDLE WINAPI 
CreateMutexW(PVOID lpMutexAttributes, BOOL bInitialOwner, PVOID lpName);
#define CreateMutex CreateMutexW

BOOL WINAPI
ReleaseMutex(HANDLE hMutex);

#define INFINITE            0xFFFFFFFF  // Infinite timeout

DWORD WINAPI
WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);

#define CONTEXT_i386    0x00010000
#define CONTEXT_CONTROL         (CONTEXT_i386 | 0x00000001L) // SS:SP, CS:IP, FLAGS, BP
#define CONTEXT_INTEGER         (CONTEXT_i386 | 0x00000002L) // AX, BX, CX, DX, SI, DI
#define CONTEXT_SEGMENTS        (CONTEXT_i386 | 0x00000004L) // DS, ES, FS, GS
#define CONTEXT_FLOATING_POINT  (CONTEXT_i386 | 0x00000008L) // 387 state
#define CONTEXT_DEBUG_REGISTERS (CONTEXT_i386 | 0x00000010L) // DB 0-3,6,7
#define CONTEXT_EXTENDED_REGISTERS  (CONTEXT_i386 | 0x00000020L) // cpu specific extensions
#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_INTEGER |\
                      CONTEXT_SEGMENTS)

BOOL WINAPI
GetThreadContext(HANDLE hThread, PCONTEXT lpContext);

BOOL WINAPI
SetThreadContext(HANDLE hThread, PCONTEXT lpContext);

DWORD WINAPI
GetCurrentThreadId(VOID);

#define SYNCHRONIZE                      (0x00100000L)
#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)
#define THREAD_ALL_ACCESS         (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0x3FF)

HANDLE WINAPI 
OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);

HANDLE WINAPI
CreateEventW(PVOID lpEventAttributes, BOOL bManualReset, BOOL bInitialState, PVOID lpName);
#define CreateEvent CreateEventW

BOOL WINAPI 
SetEvent(HANDLE hEvent);

#endif
