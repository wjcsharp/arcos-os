/*

Module Name:

    bugcheck.c

Abstract:

    ARCOS's very own blue screen of death implementation.

Author:

    Michal Strehovsky

Revision History:

*/

#include <arcos.h>
#include <ke.h>
#include <hal.h>

VOID
KeBugCheck(
    PCHAR messageText
    )
{
    //
    // Disable interrupts
    //
    KeDisableInterrupts();

    //
    // Set colors & clear the screen
    //
    HalSetBackgroundColor(4);   // blue
    HalSetForegroundColor(15);  // bright white
    HalClearDisplay();

    //
    // Print bugcheck header
    //
    HalDisplayString("\n *** STOP 0x80000001\n"); // maybe we should promote bugcode number to a parameter...
    HalDisplayString("\nA problem has been detected and ARCOS has been shut down to prevent damage\n");
    HalDisplayString("to your computer:\n\n");

    //
    // Print message
    //
    HalDisplayString(messageText);

    // TODO: print stacktrace (when I have nothing better to do...)

    //
    // Spin spin
    //
    while (1);
}
