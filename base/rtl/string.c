/*

Module Name:

    string.c

Abstract:

    Common string handling routines.

Author:

    Michal Strehovsky

Revision History:

*/

#include <types.h>

PCHAR
RtlCopyString(
    PCHAR dest,
    PCHAR src
    )
{
    char *cp = dest;

    while ((*cp++ = *src++));

    return dest;
}

LONG 
RtlCompareStrings(
    PCHAR src,
    PCHAR dest
    )
{
    int result = 0 ;

    while (*dest && !((result = (*(PUCHAR)src - *(PUCHAR)dest))))
        ++src, ++dest;

    if (result < 0)
        result = -1 ;
    else if (result > 0)
        result = 1 ;

    return result;
}

ULONG
RtlStringLength(
    PCHAR str
    )
{
    PCHAR end = str;

    while (*end++);

    return (ULONG)(end - str - 1);
}

static ULONG
_snprintchar(
    PCHAR *str,
    PULONG size,
    CHAR c
    )
{
    if (*size > 0) {
        **str = c;
        (*str)++;
        (*size)--;
    }

    return 1;
}
    
static ULONG
_snprintstring(
    PCHAR *str,
    PULONG size,
    PCHAR s
    )
{
    ULONG length = 0;
    while (*s)
        length += _snprintchar(str, size, *s++);

    return length;
}

static ULONG
_snprintlong(
    PCHAR *str,
    PULONG size,
    LONG n
    )
{
    ULONG length = 0;
    
    if (n < 0) {
        n = -n;
        length += _snprintchar(str, size, '-');
    }

    if (n / 10)
        length += _snprintlong(str, size, n / 10);

    length += _snprintchar(str, size, (n % 10) + '0');

    return length;
}
 
static ULONG
_snprintulong(
    PCHAR *str,
    PULONG size,
    ULONG n
    )
{
    ULONG length = 0;
    
    if (n / 10)
        length += _snprintulong(str, size, n / 10);

    length += _snprintchar(str, size, (n % 10) + '0');

    return length;
}

static ULONG
_snprinthex(
    PCHAR *str,
    PULONG size,
    ULONG n
    )
{
    ULONG length = 0;
    
    if (n >> 4)
        length += _snprinthex(str, size, n >> 4);

    n &= 0xF;

    if (n < 10)
        length += _snprintchar(str, size, (CHAR)n + '0');
    else
        length += _snprintchar(str, size, (CHAR)n + 'A' - 10);

    return length;
}

LONG
RtlFormatString(
    PCHAR str,
    ULONG size,
    PCHAR format,
    ...
    )
{
    ULONG length = 0;
    PCHAR cp = str;
    ULONG availablesize = size > 0 ? size - 1 : 0;

    // BUGBUG: directly touching the stack - should be replaced with va_start when we have it
    PCHAR argument = (PCHAR)&format + sizeof(PCHAR);

    while (*format) {
        if (*format == '%') {
            format++;

            switch (*format) {
                case 'd':
                    length += _snprintlong(&cp, &availablesize, *((PLONG)argument));
                    argument += sizeof(LONG);
                    break;
                case 'c':
                    length += _snprintchar(&cp, &availablesize, *((PCHAR)argument));
                    argument += sizeof(LONG);
                    break;
                case 'x':
                    length += _snprinthex(&cp, &availablesize, *((PLONG)argument));
                    argument += sizeof(LONG);
                    break;
                case 'u':
                    length += _snprintulong(&cp, &availablesize, *((PULONG)argument));
                    argument += sizeof(LONG);
                    break;
                case 's':
                    length += _snprintstring(&cp, &availablesize, *((PCHAR*)argument));
                    argument += sizeof(LONG);
                    break;
                
                default:
                    length += _snprintchar(&cp, &availablesize, '%');
                    length += _snprintchar(&cp, &availablesize, *format);
            }
            
        } else
            length += _snprintchar(&cp, &availablesize, *format);

        format++;
    }

    *cp = 0;
    
    return length;
}
