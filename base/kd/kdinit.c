/*

Module Name:

    kdinit.c

Abstract:

    Kernel debugger initialization routines.

Author:

    Michal Strehovsky

Revision History:

*/

#include <arcos.h>

#include "kdp.h"


VOID
KdInitialize(
    VOID
    )
{
    KdpPrintString("ARCOS kernel debugger\n\n");
    KdpPrintDebuggerPrompt();
}

