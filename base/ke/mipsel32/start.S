/*

Module Name:

    start.S

Abstract:

    Kernel entry point. This is where execution starts.

Author:

    Michal Strehovsky

Revision History:

*/

#include <mips/regdef.h>


    #
    # Data segment
    #
    .data
    
    # initial stack will be set up here
    .space 8192
stack:


    #
    # Code segment
    #
    .text
    
    .globl  _start
    .globl  KeInitialize
    
_start:
    #
    #       prepare status register
    #
    mfc0    t0, status

    la      t1, 0xFFBF00E8      # set up and-mask for status register
    and     t0, t1              # disable BEV, UM, ERL, EXL, IE and all interrupt lines

    la      t1, 0x10008400
    or      t0, t1              # enable all useful interrupt lines and CP0
    
    mtc0    t0, status
    mtc0    zero, cause

    #
    #       transfer control to KeInitialize
    #
    la      gp, 0x80000000
    la      sp, stack-32        # BUGBUG: why is the 32 here?
    j       KeInitialize

    # KeInitialize should NEVER return
    # this is just to ease decompilation in IDA
here:
    j       here
    
