

#include <mips/regdef.h>

    .text
    .globl KeYieldProcessor
    .globl KeDisableInterrupts
    .globl KeRestoreInterrupts

KeYieldProcessor:
    wait
    jr      ra

KeDisableInterrupts:
    mfc0    t0, status          # load status register
    
    andi    v0, t0, 1           # get interrupt enable bit

    ori     t1, zero, 1         # t1 = 1
    nor     t1, t1              # t1 = ~t1
    
    and     t0, t1              # clear interrupt enable bit
    
    mtc0    t0, status          # save result to status register
    jr      ra

KeRestoreInterrupts:
    mfc0    v0, status
    or      v0, a0              # set interrupt enable bit
    mtc0    v0, status
    jr      ra
    
