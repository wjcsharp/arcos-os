#ifndef __KE_H__
#define __KE_H__

#include <arcos.h>

VOID
KeBugCheck(PCHAR messageText);

BOOL
KeDisableInterrupts(VOID);

VOID
KeRestoreInterrupts(BOOL enable);

VOID
KeYieldProcessor(VOID);

extern PPROCESS KeCurrentProcess;

#endif
