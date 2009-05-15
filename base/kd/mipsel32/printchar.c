#include <arcos.h>

// ugly hack
#include "../../hal/mipsel32/halp.h"

#define DEBUG_CONSOLE_BASE ((PNS16550)0xb80002f8)

VOID
KdpPrintChar(
    CHAR c
    )
{
    volatile PNS16550 console = DEBUG_CONSOLE_BASE;

    // wait for the device
    while (!console->lsr.field.thre);

    // output character
    console->thr = c;

    if (c == '\n') {
        while (!console->lsr.field.thre);
        console->thr = '\r';
    }
}



