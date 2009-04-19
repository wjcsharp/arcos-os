/*

Module Name:

    display.c

Abstract:

    Win32 HAL display management functions.

Author:

    Michal Strehovsky

Revision History:

*/

#include <types.h>
#include <rtl.h>
#include <win32/minwin.h>

HANDLE hConsoleIn;
HANDLE hConsoleOut;

WORD CurrentConsoleAttribute = 0x07;

VOID
HalDisplayString(
    PCHAR string
    )
{
    DWORD bytesWritten;
    
    WriteFile(hConsoleOut, string, RtlStringLength(string), &bytesWritten, 0);
}

VOID
HalSetForegroundColor(
    UCHAR color
    )
{
    CurrentConsoleAttribute &= 0xF0;
    CurrentConsoleAttribute |= color;
    SetConsoleTextAttribute(hConsoleOut, CurrentConsoleAttribute);
}

VOID
HalSetBackgroundColor(
    UCHAR color
    )
{
    CurrentConsoleAttribute &= 0x0F;
    CurrentConsoleAttribute |= (color << 4);
    SetConsoleTextAttribute(hConsoleOut, CurrentConsoleAttribute);
}

VOID
HalClearDisplay(
    VOID
    )
{
    // dummy
}






