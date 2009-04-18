#ifndef __HAL_H__
#define __HAL_H__

#include <types.h>

VOID
HalSetForegroundColor(UCHAR color);

VOID
HalSetBackgroundColor(UCHAR color);

VOID
HalClearDisplay(VOID);

VOID
HalDisplayString(PCHAR string);

VOID
HalInitialize(VOID);

PVOID
HalGetFirstUsableMemoryAddress(VOID);

#endif
