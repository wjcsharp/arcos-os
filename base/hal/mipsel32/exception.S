/*

Module Name:

    exception.S

Abstract:

    MIPS32 HAL low level exception handling code.

Author:

    Michal Strehovsky

Revision History:

*/

#include <mips/regdef.h>

    .data

stackGuard:
    .word   0xDEADBEEF

    #       16 kB of space for kernelmode stack
    .space  16384
exceptionStack:

    #       don't allow assembler to muck AT
    .set    noat

    .text

    .globl  HalExceptionHandlerVector
    .globl  HalExceptionHandlerVectorEnd
HalExceptionHandlerVector:
    #    
    #       General exception handler.
    #       This gets copied to the appropriate location in memory.
    #
    mfc0    k0, cause
    
    #       HACKHACK: acknowledge a hardware 0 interrupt for the __hacked__ QEMU.
    #       The interrupt should be somehow acknowledged using the i8259A controller
    #       but I have no idea how to do it, so I just clear the interrupt bit.
    #       Simics has a workaround for this that does not conform to the MIPS
    #       specification either. You need a modified QEMU as clearing interrupt bits
    #       is not really supported.
    #       This hack also features a race condition if something happens between mfc0 and
    #       mtc0, but whatever. On Simics it will run just fine.
    la      k1, ~0x0400
    and     k1, k0
    mtc0    k1, cause
    #       end of hack
    
    move    k1, sp                  # save stack pointer to k1
    la      sp, exceptionStack      # set up exception stack

    #
    #       TODO: Check stack for corruption
    #
    #       
    
    j       HalExceptionHandler     # jump to the long handler
HalExceptionHandlerVectorEnd:




    .text
HalExceptionHandler:
    #    
    #       At this point:    
    #       Interrupts are off.
    #       k0 contains the cause
    #       k1 contains the old stack pointer.
    #       sp points into the kernel stack.
    #       All other registers are untouched.
    #
    addi    sp, -(16 + (4 * 36))        # make space for EXCEPTION_FRAME
                                        # and argument spill zone

    #
    #       store context
    #
    sw      AT, 16(sp)
    sw      v0, 20(sp)
    sw      v1, 24(sp)

    sw      a0, 28(sp)
    sw      a1, 32(sp)
    sw      a2, 36(sp)
    sw      a3, 40(sp)

    sw      t0, 44(sp)
    sw      t1, 48(sp)
    sw      t2, 52(sp)
    sw      t3, 56(sp)
    sw      t4, 60(sp)
    sw      t5, 64(sp)
    sw      t6, 68(sp)
    sw      t7, 72(sp)

    sw      s0, 76(sp)
    sw      s1, 80(sp)
    sw      s2, 84(sp)
    sw      s3, 88(sp)
    sw      s4, 92(sp)
    sw      s5, 96(sp)
    sw      s6, 100(sp)
    sw      s7, 104(sp)

    sw      t8, 108(sp)
    sw      t9, 112(sp)

    #       K0 and K1 are our scratch registers, so don't bother saving them

    sw      gp, 124(sp)
    sw      k1, 128(sp)     # k1 is the old sp
    sw      fp, 132(sp)
    sw      ra, 136(sp)
    
    mfhi    t0
    mflo    t1
    mfc0    t2, epc
    sw      t0, 140(sp)     # store Hi
    sw      t1, 144(sp)     # store Lo
    sw      t2, 148(sp)     # store Pc
    
    #
    #       set up the rest of exception frame
    #
    mfc0    t0, status
    sw      k0, 152(sp)     # store cause
    sw      t0, 156(sp)     # store status

    #
    #       call high level handler
    #
    .globl  HalHandleException
    addi    a0, sp, 16
    jal     HalHandleException

    #
    #       reload context - special variables
    #
    lw      t0, 140(sp)     # load Hi
    lw      t1, 144(sp)     # load Lo
    lw      t2, 148(sp)     # load Pc
    lw      t3, 152(sp)     # load cause
    lw      t4, 156(sp)     # load status
    mthi    t0              # set Hi
    mtlo    t1              # set Lo
    mtc0    t2, epc         # set epc
    mtc0    t4, status      # set status

    #       HACKHACK: QEMU HW interrupt 0 hack again
    la      t2, ~0x400
    and     t3, t2
    mfc0    t0, cause
    andi    t0, 0x400
    or      t3, t0
    #       end of hack
    
    mtc0    t3, cause       # set cause
    
    #
    #       reload rest of context
    #
    lw      AT, 16(sp)
    lw      v0, 20(sp)
    lw      v1, 24(sp)

    lw      a0, 28(sp)
    lw      a1, 32(sp)
    lw      a2, 36(sp)
    lw      a3, 40(sp)

    lw      t0, 44(sp)
    lw      t1, 48(sp)
    lw      t2, 52(sp)
    lw      t3, 56(sp)
    lw      t4, 60(sp)
    lw      t5, 64(sp)
    lw      t6, 68(sp)
    lw      t7, 72(sp)

    lw      s0, 76(sp)
    lw      s1, 80(sp)
    lw      s2, 84(sp)
    lw      s3, 88(sp)
    lw      s4, 92(sp)
    lw      s5, 96(sp)
    lw      s6, 100(sp)
    lw      s7, 104(sp)

    lw      t8, 108(sp)
    lw      t9, 112(sp)

    #       K0 and K1 are our scratch registers, so don't bother loading them

    lw      gp, 124(sp)
    lw      k1, 128(sp)     # k1 is the old sp
    lw      fp, 132(sp)
    lw      ra, 136(sp)
    
    #
    #       restore stack pointer
    #
    move    sp, k1

    #
    #       go back to the state before exception occured
    #
    eret

