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

extern POBJECT_TYPE processType;

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

STATUS
PsInitialize() {
    STATUS status;
    status = CreateProcessObjectType(processType);
    return status;
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

    status = ObCreateObject(processType, 0, sizeof (PROCESS), createdProcessObject);
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
    KeEnqueue(process); //Shouldnt this return a status?
    ObDereferenceObject(createdProcessObject);
    return STATUS_SUCCESS;
}

STATUS
PsKillProcess(
        PPROCESS process
        ) {
    STATUS status;

    status = ObReferenceObject(process, processType);
    if (status != 0)
        return status;

    KeDequeue(process);
    ObKillProcess(process);
    MmFree(process->AllocatedMemory);
    ObDereferenceObject(process);
    return STATUS_SUCCESS;
}

STATUS
PsGetProcessExitStatus(
        HANDLE psHandle,
        PULONG exitStatus
        ) {
    //Get process exit status
    PPROCESS process;
    STATUS status;

    status = ObReferenceObjectByHandle(psHandle, processType, (void**) & process);
    if (status != 0) return status;

    *exitStatus = process->ExitStatus;
    ObDereferenceObject(process);
    return STATUS_SUCCESS;
}