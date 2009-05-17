#ifndef __KE_H__
#define __KE_H__

#include <arcos.h>


VOID
KeBugCheck(PCHAR MessageText);

BOOL
KeDisableInterrupts(VOID);

VOID
KeRestoreInterrupts(BOOL Enable);

VOID
KeYieldProcessor(VOID);

extern PPROCESS KeCurrentProcess;

#define PROCESS_PRIORITY_LEVELS 32

//
// Holds heads and tails of process scheduling queues
//
typedef struct _PROCESS_QUEUE
{
    PPROCESS First;
    PPROCESS Last;
} PROCESS_QUEUE, *PPROCESS_QUEUE;

STATUS 
KeStartSchedulingProcess(PPROCESS Process);

STATUS 
KeStopSchedulingProcess(PPROCESS Process);

VOID 
KeChangeProcessPriority(PPROCESS Process, ULONG NewPriority);

VOID
KeBlockProcess(VOID);

VOID
KeResumeProcess(PPROCESS Process);

VOID
KeSuspendProcess(ULONG Milliseconds);

ULONG
KeGetTickCount(VOID);

VOID
KeCaptureContext(PCONTEXT context);

VOID
KeRestoreContext(PCONTEXT context);

VOID
KeHandleTimer(VOID);

#endif
