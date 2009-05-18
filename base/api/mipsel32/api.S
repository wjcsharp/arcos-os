#include <mips/regdef.h>

    .text

    .globl  Sleep
Sleep:
    addi    v0, zero, 0
    syscall
    jr      ra

    .globl  GetProcessId
GetProcessId:
    addi    v0, zero, 1
    syscall
    jr      ra

.globl  KillByPid
KillByPid:
    addi    v0, zero, 2
    syscall
    jr      ra

    # Put your own syscalls here. Just change the name and increment the 0.

    