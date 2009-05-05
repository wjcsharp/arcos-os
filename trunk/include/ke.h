/*REVISIONS: 
Magnus added #include <ps.h> for PPROCESS
*/

#ifndef __KE_H__
#define __KE_H__

#include <arcos.h>
#include <ps.h>

VOID
KeBugCheck(PCHAR messageText);

BOOL
KeDisableInterrupts(VOID);

VOID
KeRestoreInterrupts(BOOL enable);

VOID
KeYieldProcessor(VOID);

extern PPROCESS KeCurrentProcess;

#ifndef __QUEUE__
#define __QUEUE__

#define MAXIMUM_QUEUE_LEVEL 32

typedef struct _QUEUE
{
	PPROCESS First;
	PPROCESS Last;
} QUEUE, *PQUEUE;
QUEUE priorities [MAXIMUM_QUEUE_LEVEL];

VOID 
KeEnqueue(PPROCESS object);

VOID 
KeDequeue(PPROCESS object);

VOID 
KeChangeProcessPriority(PPROCESS object, ULONG newPriority);

STATUS 
ProcSleep(PHANDLE PsHandle, ULONG Time);

#endif //QUEUE
#endif
