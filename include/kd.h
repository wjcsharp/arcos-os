#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef HAVE_KD

#include <arcos.h>

VOID
KdPrint(PCHAR format,...);

VOID
KdCharacterReceived(CHAR c);

VOID
KdInitialize(VOID);

#else

#define KdPrint(format, args...) ((void)0)

#endif

#endif
