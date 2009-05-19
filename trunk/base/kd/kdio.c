/*

Module Name:

    kdio.c

Abstract:

    Kernel debugger I/O routines.

Author:

    Michal Strehovsky

Revision History:

*/

#include <arcos.h>
#include <stdarg.h>

#include "kdp.h"

CHAR commandBuffer[160];

ULONG commandBufferPosition;

VOID
KdpPrintString(
    PCHAR s
    )
{
    while (*s)
        KdpPrintChar(*s++);
}

VOID
KdpPrintLong(
    LONG n
    )
{
    if (n < 0) {
        n = -n;
        KdpPrintChar('-');
    }

    if (n / 10)
        KdpPrintLong(n / 10);

    KdpPrintChar((n % 10) + '0');
}

VOID
KdpPrintUlong(
    ULONG n
    )
{
    if (n / 10)
        KdpPrintUlong(n / 10);

    KdpPrintChar((n % 10) + '0');
}

VOID
KdpPrintHex(
    ULONG n
)
{
    if (n >> 4)
        KdpPrintHex(n >> 4);

    n &= 0xF;

    if (n < 10)
        KdpPrintChar((CHAR)n + '0');
    else
        KdpPrintChar((CHAR)n + 'A' - 10);
}

VOID
KdpPrintEx(
    PCHAR format,
    va_list args
    )
{
    while (*format) {
        if (*format == '%') {
            format++;

            switch (*format) {
                case 'd':
                    KdpPrintLong(va_arg(args, LONG));
                    break;
                case 'c':
                    KdpPrintChar(va_arg(args, int));
                    break;
                case 'x':
                    KdpPrintHex(va_arg(args, ULONG));
                    break;
                case 'u':
                    KdpPrintUlong(va_arg(args, ULONG));
                    break;
                case 's':
                    KdpPrintString(va_arg(args, PCHAR));
                    break;
                
                default:
                    KdpPrintChar('%');
                    KdpPrintChar(*format);
            }
            
        } else
            KdpPrintChar(*format);

        format++;
    }
}

VOID
KdpPrint(
    PCHAR format,
    ...
    )
{
    va_list args;
    va_start(args, format);
    KdpPrintEx(format, args);
    va_end(args);
}

VOID
KdPrint(
    PCHAR format,
    ...
    )
{
    va_list args;

    //KdpPrintChar('\n');
    
    va_start(args, format);
    KdpPrintEx(format, args);
    va_end(args);

    KdpPrintChar('\n');
    KdpPrintDebuggerPrompt();
    commandBuffer[commandBufferPosition] = 0;
    KdpPrintString(commandBuffer);
}

VOID
KdCharacterReceived(
    CHAR c
    )
{
    if (c == '\r') {
        commandBuffer[commandBufferPosition] = 0;
        commandBufferPosition = 0;
        KdpPrintChar('\n');
        KdpExecuteCommand(commandBuffer);
        KdpPrintDebuggerPrompt();
        return;
    }

    if (c == '\b' || c == 127) {
        if (commandBufferPosition > 0) {
            commandBufferPosition--;
            KdpPrintString("\e[D \e[D");
        }

        return;
    }

    if ((c > 31 && c < 127) && (commandBufferPosition < sizeof(commandBuffer) - 1)) {
        KdpPrintChar(c);
        commandBuffer[commandBufferPosition] = c;
        commandBufferPosition++;
    }
}

VOID
KdpPrintDebuggerPrompt(
    VOID
    )
{
    KdpPrintString("kd> ");
}

