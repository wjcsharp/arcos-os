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

.globl  CreateFile
CreateFile:
    addi    v0, zero, 5
    syscall
    jr      ra

.globl  WriteFile
WriteFile:
    addi    v0, zero, 6
    syscall
    jr      ra

.globl  ReadFile
ReadFile:
    addi    v0, zero, 7
    syscall
    jr      ra

.globl  KillByPID
KillByPID:
    addi    v0, zero, 8
    syscall
    jr      ra

.globl  ChangePriority
ChangePriority:
    addi    v0, zero, 9
    syscall
    jr      ra

.globl  SendMessage
SendMessage:
    addi    v0, zero, 10
    syscall
    jr      ra

.globl  ReceiveFirst
ReceiveFirst:
    addi    v0, zero, 11
    syscall
    jr      ra

.globl  ReceiveHighestPriority
ReceiveHighestPriority:
    addi    v0, zero, 12
    syscall
    jr      ra

.globl  ReceiveByType
ReceiveByType:
    addi    v0, zero, 13
    syscall
    jr      ra

.globl  GetMessageSize
GetMessagesize:
    addi    v0, zero, 14
    syscall
    jr      ra

.globl  CopyMessage
CopyMessage:
    addi    v0, zero, 15
    syscall
    jr      ra

.globl  DeleteMessage
DeleteMessage:
    addi    v0, zero, 16
    syscall
    jr      ra

.globl  GetProcessInfo
GetProcessInfo:
    addi    v0, zero, 17
    syscall
    jr      ra

.globl SuperviseProc
SuperviseProc:
    addi    v0, zero, 18
    syscall
    jr      ra
    
.globl Malloc
Malloc:
	addi    v0, zero, 19
	syscall
	jr      ra

.globl Free
Free:
	addi    v0, zero, 20
	syscall
	jr      ra
	
.globl FreeAll
FreeAll:
	addi    v0, zero, 21
	syscall
	jr      ra
	
.globl GetUsedMemSum
GetUsedMemSum:
	addi    v0, zero, 22
	syscall
	jr      ra

.globl WaitForSingleObject
WaitForSingleObject:
    addi    v0, zero, 23
    syscall
    jr      ra

    # Put your own syscalls here. Just change the name and increment the 0.

