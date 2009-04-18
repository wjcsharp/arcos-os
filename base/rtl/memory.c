/*

Module Name:

    memory.c

Abstract:

    Run time library routines for handling memory.

Author:

    Michal Strehovsky

Revision History:

*/

#include <types.h>

PVOID 
RtlCopyMemory(
    PVOID dest,
    PVOID src,
    ULONG count
    )
{
    PCHAR s = (PCHAR)src;
    PCHAR d = (PCHAR)dest;

    while (count--) *d++ = *s++;

    return dest;
}

PVOID
RtlFillMemory(
    PVOID dest,
    CHAR val,
    ULONG count
    )
{
    PCHAR d = (PCHAR)dest;

    while (count--) *d++ = val;

    return dest;
}

