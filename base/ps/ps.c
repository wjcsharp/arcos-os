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
#include <hal.h>
#include <kd.h>
//#define DEBUG_PS
//
//List of all available applications in the OS
//
POBJECT_TYPE processType;

APPLICATION PsAvailApps[] = {
    {"MyFirstProgram", MyFirstProgram},
    {"TestProcess3", PSTestProcess3}
};


BOOL
PIDInUse(ULONG PID);

ULONG
GetPID();

STATUS
CreateProcessObjectType() {
    //Create Process object type
    OBJECT_TYPE_INITIALIZER typeInitializer;
    STATUS status;
    //
    // create a new object type: Process
    //
    typeInitializer.DumpMethod = NULL; //Should be implemented...
    typeInitializer.DeleteMethod = NULL;
    status = ObCreateObjectType(0x0CE55, &typeInitializer, &processType);
    return status;
}

VOID
PsInitialize() {


    PVOID createdProcessObject = NULL;
    PPROCESS process = NULL;


    //Create Process Object Type
    OBJECT_TYPE_INITIALIZER typeInitializer;
    typeInitializer.DumpMethod = NULL; //Should be implemented...
    typeInitializer.DeleteMethod = NULL;
    ObCreateObjectType(0x0CE55, &typeInitializer, &processType);
    //
    //Create initial process
    //
    ObCreateObject(processType, 0, sizeof (PROCESS), &createdProcessObject);
    ASSERT(createdProcessObject);

    //Cast to PPROCESS before using the new object
    process = (PPROCESS) createdProcessObject;
    ASSERT(process);

    //Zero memory of process
    RtlZeroMemory(process, sizeof (PROCESS));

    //Set quantum
    process->Quantum = 1;

    //Initialize handletable
    ObInitProcess(NULL, process);
    //Set current process to the newly created
    KeCurrentProcess = process;
    ASSERT(KeCurrentProcess);
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

VOID
MyFirstProgram() {
    ULONG arb = 27;
    arb = arb + 123;
    KdPrint("My first process executed %d", arb);
    //PsKillMe();
}

VOID
PSTestProcess3() {
    ULONG i;

    KdPrint("Hello from testprocess2");

    while (1) {
        for (i = 0; i < 0x1FFFFFF; i++);
        KdPrint("testprocess3 heartbeat");
    }
}

VOID
PsKillMe() {
    STATUS status;
    status = PsKillProcess(KeCurrentProcess, 0);
    if (0 != status)
        KdPrint("KillMe failed");
}

PVOID
GetProgramAdress(PCHAR ProgramName) {
    ULONG index = 0;

    for (index = 0; index < (sizeof (PsAvailApps) / sizeof (APPLICATION)); ++index) {
        if (0 == (RtlCompareStrings(PsAvailApps[index].Name, ProgramName)))
            return PsAvailApps[index].Execute;
    }
    return 0;
}

STATUS
PsCreateProcessByName(
        PCHAR ProgramName,
        ULONG Priority,
        PHANDLE ProcessHandle,
        PCHAR Args
        ) {
    STATUS status;
    status = PsCreateProcess(GetProgramAdress(ProgramName), Priority, ProcessHandle, Args);
    return status;
}

STATUS
PsCreateProcess(
        VOID(*PStartingAddress)(),
        ULONG Priority,
        PHANDLE ProcessHandle,
        PCHAR Args
        ) {
    //Create new process
    STATUS status = 0;
    PVOID memPointer;
    PVOID createdProcessObject = NULL;
    PPROCESS process = NULL;

    // Sanitycheck process Adress
    if (NULL == PStartingAddress)
        return STATUS_INVALID_APPLICATION_ADRESS;

    //Create process object
    status = ObCreateObject(processType, 0, sizeof (PROCESS), &createdProcessObject);
    if (status != 0) return status;
    ASSERT(createdProcessObject);

    //Cast to PPROCESS before using the new object
    process = (PPROCESS) createdProcessObject;
    ASSERT(process);

    //Zero memory of process
    RtlZeroMemory(process, sizeof (PROCESS));

#ifdef DEBUG_PS
    KdPrint("after zero memory");
#endif
    //Set process status to created
    process->State = created;

    //Ask mamory manager for a chunk of memory to be attached to the process
    memPointer = MmAlloc(PROCESS_MEMORY_SIZE);
    if (memPointer == NULL) {
        ObDereferenceObject(createdProcessObject);
        return STATUS_NO_MEMORY;
    }
#ifdef DEBUG_PS
    KdPrint("malloc done");
#endif
    //Attach memory block
    process->AllocatedMemory = memPointer;
    process->PID = GetPID();
    process->Priority = Priority;
    process->CPUTime = 0;
    process->Args = Args;
    process->ExitStatus = 654321; //Runningprocess (CRASH exit status)
    //---Initialize Context what needs to be init?
    (process->Context).Pc = (ULONG) PStartingAddress;
    (process->Context).Sp = (ULONG) ((PCHAR) memPointer + PROCESS_MEMORY_SIZE);

#ifdef DEBUG_PS
    KdPrint("attach mem init pobject, context");
#endif

    //Initialize handletable
    status = ObInitProcess(KeCurrentProcess, process);
    if (status != 0) {
        ObDereferenceObject(createdProcessObject);
        MmFree(memPointer);
        return status;
    }
#ifdef DEBUG_PS
    KdPrint("initialized handletable");
#endif
    //Create Handle to object
    status = ObOpenObjectByPointer(createdProcessObject, 0, processType, ProcessHandle);
    if (status != 0) {
        ObDereferenceObject(createdProcessObject);
        MmFree(memPointer);
        return status;
    }
    process->State = ready;
#ifdef DEBUG_PS
    KdPrint("attached handle");
#endif

    //Schedule process
    status = KeStartSchedulingProcess(process);
    if (status != 0) {
        ObDereferenceObject(createdProcessObject);
        MmFree(memPointer);
        return status;
    }
#ifdef DEBUG_PS
    KdPrint("scheduled process");
#endif
    ObDereferenceObject(process/*createdProcessObject*/);
#ifdef DEBUG_PS
    KdPrint("dereferenced object");
#endif
    return STATUS_SUCCESS;
}

STATUS
PsKillProcess(
        PPROCESS PProcess,
        ULONG ExitStatus
        ) {
    STATUS status;

    status = ObReferenceObject(PProcess, processType);
    if (status != 0)
        return status;

    status = KeStopSchedulingProcess(PProcess);
    if (status != 0) return status;

    ObKillProcess(PProcess);
    MmFree(PProcess->AllocatedMemory);

#ifdef DEBUG_PS
    KdPrint("PsKillP freed memory");
#endif
    PProcess->ExitStatus = ExitStatus;
    //BUGBUGBUGBUGBUGBUG
    //Free message-queue
    ObDereferenceObject(PProcess);
#ifdef DEBUG_PS
    KdPrint("PsKillP done");
#endif
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
        PPROCESS_STATE PState
        ) {
    //Get process state
    PPROCESS pprocess;
    STATUS status;

    status = ObReferenceObjectByHandle(ProcessHandle, processType, (void**) & pprocess);
    if (status != 0)
        return status;

    *PState = pprocess->State;
    ObDereferenceObject(pprocess);
    return STATUS_SUCCESS;
}

STATUS
PsGetPid(
        PHANDLE PHandle,
        PULONG PPid
        ) {
    PPROCESS pprocess;
    STATUS status;

    status = ObReferenceObjectByHandle(PHandle, processType, (void**) & pprocess);
    if (status != 0)
        return status;

    *PPid = pprocess->PID;
    ObDereferenceObject(pprocess);
    return STATUS_SUCCESS;
};

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
        process = ObGetNextObjectOfType(process);
    }
    return STATUS_NO_SUCH_PROCESS;
}

STATUS
PsReferenceProcess(
        ULONG PID,
        PPROCESS * ProcessPtr
        ) {
    PPROCESS process;

    process = ObGetFirstObjectOfType(processType);

    while (process) {
        if (process->PID == PID) {
            *ProcessPtr = process;
            ObReferenceObject(process, processType);
            return STATUS_SUCCESS;
        }
        process = ObGetNextObjectOfType(process);
    }
    *ProcessPtr = NULL;
    return STATUS_NO_SUCH_PROCESS;
}


STATUS
CopyPInfo(
        PPROCESS Process,
        PPROCESS_INFO Info
        );

STATUS
CopyPInfo(
        PPROCESS Process,
        PPROCESS_INFO Info) {
    if (NULL == Process)
        return STATUS_INVALID_PARAMETER;

    Info->CPUTime = Process->CPUTime;
    Info->Priority = Process->Priority;

    Info->State = Process->State;
    Info->RunningProgram = Process->RunningProgram;
    Info->PID = Process->PID;
    return STATUS_NO_SUCH_PROCESS;
};

STATUS
PsGetProcessesInfo(
        PROCESS_INFO Buffer[],
        ULONG BufferSize,
        PULONG NumberProcesses
        ) {
    PROCESS_INFO pinfo;
    PPROCESS pprocess;
    ULONG foundProc = 0;
    STATUS status;

    //Get first process object
    pprocess = ObGetFirstObjectOfType(processType);

    while (pprocess) {
        if (BufferSize > foundProc) {
            status = CopyPInfo(pprocess, &pinfo);
            if (0 != status)
                return status;
        }
        Buffer[foundProc] = pinfo;
        foundProc++;
        pprocess = ObGetNextObjectOfType(pprocess);
    }
    *NumberProcesses = foundProc;
    return STATUS_SUCCESS;
}

