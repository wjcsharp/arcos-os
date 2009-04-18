#ifndef __RTL_H__
#define __RTL_H__

PCHAR
RtlCopyString(PCHAR dest, PCHAR src);

LONG 
RtlCompareStrings(PCHAR src, PCHAR dest);

ULONG
RtlStringLength(PCHAR str);

LONG
RtlFormatString(PCHAR str, ULONG size, const PCHAR format,...);

PVOID 
RtlCopyMemory(PVOID dest, PVOID src, ULONG count);

PVOID
RtlFillMemory(PVOID dest, CHAR val, ULONG count);

#define RtlZeroMemory(dest, count)  RtlFillMemory(dest, 0, count)

#endif
