/* 
 * File:   ps.c
 * Author: Magnus Söderling
 *
 */

#include <arcos.h>
#include <ps.h>
#include <mm.h>
#include <ob.h>
#include <ke.h>
#include <rtl.h>

BOOL
PIDInUse(ULONG PID);

ULONG
GetPID();

POBJECT_TYPE processType;

STATUS
CreateProcessObjectType(
        POBJECT_TYPE ProcessType
        ) {
    //Create Process object type
    OBJECT_TYPE_INITIALIZER typeInitializer;
    STATUS status;

    //
    // create a new object type: Process
    //
    typeInitializer.DumpMethod = NULL; //Should be implemented...
    typeInitializer.DeleteMethod = NULL;
    status = ObCreateObjectType(0x0CE55, &typeInitializer, &ProcessType);

    return status;
}

VOID
PsInitialize() {
    CreateProcessObjectType(processType);
}

BOOL
PIDInUse(
        ULONG PID
        ) {
    PPROCESS process;

    process = ObGetFirstObjectOfType(processType);

    while (process) {
        if (process->PID == PID)
            return TRUE;

        process = ObGetNextObjectOfType(process);
    }
    return FALSE;
}


ULONG
GetPID() {
    ULONG PID = 1;

    while (PIDInUse(PID)) {
        PID++;
    }
    return PID;
}

STATUS
PsCreateProcess(
        PVOID(*PStartingAddress)(),
        ULONG Priority,
        PHANDLE ProcessHandle,
        PCHAR Args
        ) {
    //Create new process
    STATUS status = 0;
    PVOID memPointer;
    PVOID createdProcessObject = NULL;
    PPROCESS process = NULL;

    status = ObCreateObject(processType, 0, sizeof (PROCESS), &createdProcessObject);
    if (status != 0) return status;

    //Cast to PPROCESS before using the new object
    process = (PPROCESS) createdProcessObject;

    //Zero memory of process
    RtlZeroMemory(process, sizeof (PROCESS));

    //Set process status to created
    process->State = created;

    //Ask mamory manager for a chunk of memory to be attached to the process
    memPointer = MmAlloc(PROCESS_MEMORY_SIZE);
    if (memPointer == NULL) {
        ObDereferenceObject(createdProcessObject);
        return STATUS_NO_MEMORY;
    }
    //Attach memory block
    process->AllocatedMemory = memPointer;

    //Generate PID
    process->PID = GetPID();

    //---Initialize Context what needs to be init?
    //Set priority
    process->Priority = Priority;
    //Initialize CPUTime
    process->CPUTime = 0;

    (process->Context).Pc = (ULONG) PStartingAddress;

    (process->Context).Sp = (ULONG) (memPointer + PROCESS_MEMORY_SIZE);

    //Initialize handletable
    status = ObInitProcess(KeCurrentProcess, process);
    if (status != 0) {
        ObDereferenceObject(createdProcessObject);
        MmFree(memPointer);
        return status;
    }

    status = ObOpenObjectByPointer(createdProcessObject, 0, processType, ProcessHandle);
    if (status != 0) {
        ObDereferenceObject(createdProcessObject);
        MmFree(memPointer);
        return status;
    }
    process->State = ready;

    //Schedule process
    status = KeStartSchedulingProcess(process);
    if (status != 0) {
        ObDereferenceObject(createdProcessObject);
        MmFree(memPointer);
        return status;
    }

    ObDereferenceObject(createdProcessObject);
    return STATUS_SUCCESS;
}

STATUS
PsKillProcess(
        PPROCESS Process
        ) {
    STATUS status;

    status = ObReferenceObject(Process, processType);
    if (status != 0)
        return status;

    status = KeStopSchedulingProcess(Process);
    if (status != 0) return status;

    ObKillProcess(Process);
    MmFree(Process->AllocatedMemory);
    ObDereferenceObject(Process);
    return STATUS_SUCCESS;
}

STATUS
PsGetExitStatus(
        HANDLE ProcessHandle,
        PULONG ExitStatus
        ) {
    //Get process exit status
    PPROCESS process;
    STATUS status;

    status = ObReferenceObjectByHandle(ProcessHandle, processType, (void**) & process);
    if (status != 0)
        return status;

    *ExitStatus = process->ExitStatus;
    ObDereferenceObject(process);
    return STATUS_SUCCESS;
}

STATUS
PsGetPriority(
        HANDLE ProcessHandle,
        PULONG Priority
        ) {
    //Get process PRIORITY
    PPROCESS process;
    STATUS status;

    status = ObReferenceObjectByHandle(ProcessHandle, processType, (void**) & process);
    if (status != 0)
        return status;

    *Priority = process->Priority;
    ObDereferenceObject(process);
    return STATUS_SUCCESS;
}

STATUS
PsGetState(
        HANDLE ProcessHandle,
        PULONG State
        ) {
    //Get process state
    PPROCESS process;
    STATUS status;

    status = ObReferenceObjectByHandle(ProcessHandle, processType, (void**) & process);
    if (status != 0)
        return status;

    *State = process->Priority;
    ObDereferenceObject(process);
    return STATUS_SUCCESS;
}

STATUS
PsOpenProcess(
        ULONG PID,
        PHANDLE ProcessHandle
        ) {
    STATUS status;
    PPROCESS process;

    process = ObGetFirstObjectOfType(processType);

    while (process) {
        if (process->PID == PID) {
            status = ObOpenObjectByPointer(process, 0, processType, ProcessHandle);
            return status;
        }
        process = ObGetNextObjectOfType(processType);
    }

    return STATUS_NO_SUCH_PROCESS;
}

STATUS
PsGetRunningProcesses(
        PPROCESS Buffer[],
        ULONG BufferSize,
        PULONG NumberRunningProcesses
        ) {
    PPROCESS process;
    ULONG numRunProcesses = 0;
    STATUS status;

    //Get first process object
    process = ObGetFirstObjectOfType(processType);

    while (process) {
        if (process->State == running) {

            if (BufferSize > numRunProcesses) {
                status = ObReferenceObject(process, processType);
                //If reference was created add to buffer otherwise disregard
                if (status == STATUS_SUCCESS) {
                    Buffer[numRunProcesses] = process;
                    numRunProcesses++;
                }
            } else //buffer is full
                numRunProcesses++;
        }
        process = ObGetNextObjectOfType(processType);
    }
    *NumberRunningProcesses = numRunProcesses;
    return STATUS_SUCCESS;
}


