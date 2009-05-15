#ifndef __KDP_H__
#define __KDP_H__

VOID
KdpPrintDebuggerPrompt(VOID);

VOID
KdpExecuteCommand(PCHAR command);

VOID
KdpPrintString(PCHAR s);

VOID
KdpPrintLong(LONG n);

VOID
KdpPrintUlong(ULONG n);

VOID
KdpPrintHex(ULONG n);

VOID
KdpPrintChar(CHAR c);

VOID
KdpPrint(PCHAR format,...);

#endif
