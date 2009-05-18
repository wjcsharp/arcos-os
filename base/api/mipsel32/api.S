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

.globl  KillMe
KillMe:
    addi    v0, zero, 2
    syscall
    jr      ra

.globl  CreateProcess
CreateProcess:
    addi    v0, zero, 3
    syscall
    jr      ra

.globl  GetProcessPriority
GetProcessPriority:
    addi    v0, zero, 4
    syscall
    jr      ra

    # Put your own syscalls here. Just change the name and increment the 0.

    