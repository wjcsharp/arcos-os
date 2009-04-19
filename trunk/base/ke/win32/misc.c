#include <types.h>
#include <win32/minwin.h>

extern HANDLE hInterruptMutex;
extern HANDLE hOsThread;

BOOL
KeDisableInterrupts(
    VOID
    )
{
    WaitForSingleObject(hInterruptMutex, INFINITE);
    return TRUE; // TODO return previous state
}

VOID
KeRestoreInterrupts(
    BOOL enable
    )
{
    if (enable)
        ReleaseMutex(hInterruptMutex);
}

VOID
KeYieldProcessor(
    VOID
    )
{
}
