/*

Module Name:

    timer.S

Abstract:

    MIPS32 HAL interval timer support routines.

Author:

    Michal Strehovsky

Revision History:

*/

#include <mips/regdef.h>

    .text

    .globl  HalResetTimer
    .ent    HalResetTimer
HalResetTimer:
    mtc0    zero, count
    mtc0    a0, compare
    jr      ra
    .end    HalResetTimer
    
    
