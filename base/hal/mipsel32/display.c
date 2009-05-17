/*

Module Name:

    display.c

Abstract:

    MIPS32 HAL display management functions.

Author:

    Michal Strehovsky

Revision History:

*/

#include <arcos.h>
#include <ke.h>
#include <rtl.h>
#include "halp.h"

VOID
HalDisplayString(
    PCHAR string
    )
{
//    if (string == NULL)		// Just to get rid of that exception.
//	return;

    volatile PNS16550 tty = (PNS16550)UART_BASE;

    while (*string) {

        // wait for the device
        while (!tty->lsr.field.thre);

        tty->thr = *string;

        // new line: output both carriage return and line feed
        if (*string == '\n') {
            
            // wait for the device
            while (!tty->lsr.field.thre);
            tty->thr = '\r';
        }
      
        string++;
    }
}

VOID
HalSetForegroundColor(
    UCHAR color
    )
{
    CHAR ansiCommand[16];

    RtlFormatString(ansiCommand, sizeof(ansiCommand), "\e[%d;%dm",
        color > 7 ? 1 : 0, (color > 7 ? color - 8 : color) + 30);
    
    HalDisplayString(ansiCommand);
}

VOID
HalSetBackgroundColor(
    UCHAR color
    )
{
    CHAR ansiCommand[16];

    RtlFormatString(ansiCommand, sizeof(ansiCommand), "\e[%dm",
        color + 40);
    
    HalDisplayString(ansiCommand);
}

VOID
HalClearDisplay(
    VOID
    )
{
    HalDisplayString("\e[2J\e[H");
}





