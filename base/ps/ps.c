/* 
 * File:   ps.c
 * Author: Magnus Söderling
 *
 */

#include <arcos.h>
#include <apps.h>
#include <ps.h>
#include <mm.h>
#include <ob.h>
#include <ke.h>
#include <rtl.h>
#include <kd.h>
#include <api.h>//SYSCALLS FOR USERPROGRAMS

//#define DEBUG_PS
//
//List of all available applications in the OS
//
POBJECT_TYPE processType;

APPLICATION PsAvailApps[] = {
    {"MyFirstProgram", MyFirstProgram},
    {"TestProcess3", PSTestProcess3},
    {"Kill", Kill}
};


BOOL
PIDInUse(ULONG PID);

ULONG
GetPID();

VOID
Kill() {
    ULONG PID;
    STATUS status;
    PID = RtlAtoUL(KeCurrentProcess->Args);
         
    status = KillByPID(PID, 1);
    if (0 == status)
        KdPrint("Killed %d", PID);
    else
        KdPrint("Failed to kill %d", PID);

    KillMe();
}

VOID
MyFirstProgram() {
    KdPrint("My first process My Prio is %d.", GetProcessPriority());
    KdPrint("My first process executed and DIES %d", RtlAtoUL("   123"));
    KillMe();
}

VOID
PSTestProcess3() {
    ULONG i;
    HANDLE handtag, handtag2;

    //KdPrint("Hello from testprocess3 My PID is %d.", GetProcessId());
    //KdPrint("Hello from testprocess3 My Prio is %d.", GetProcessPriority());
    for (i = 0; i < 5; i++) {
        KdPrint("testprocess3 heartbeat");
    }
    Sleep(5000);
    KdPrint("testprocess3 Creates new PROCESS");

    KdPrint("createstatus: %d", CreateProcess("MyFirstProgram", 8, &handtag2, NULL));
    Sleep(15000);
    CreateProcess("MyFirstProgram", 8, &handtag, NULL);
    Sleep(30000);
    KdPrint("testprocess3 DIES");
    KillMe();
}

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

PVOID
GetProgramAdress(PCHAR ProgramName) {
    ULONG index;

    if (NULL == ProgramName)
        return NULL;

    for (index = 0; index < (sizeof (PsAvailApps) / sizeof (APPLICATION)); ++index) {
        if (RtlStringLength(PsAvailApps[index].Name) == RtlStringLength(ProgramName)) {
            if (0 == RtlCompareStrings(PsAvailApps[index].Name, ProgramName))
                return PsAvailApps[index].Execute;
        }
    }
    return NULL;
}

STATUS
PsCreateProcessByName(
        PCHAR ProgramName,
        ULONG Priority,
        PHANDLE ProcessHandle,
        PCHAR Args
        ) {
    STATUS status;
    PVOID programAdress;

    //Get program adress
    programAdress = GetProgramAdress(ProgramName);
    if (NULL == programAdress)
        return STATUS_NO_SUCH_PROGRAM;

    status = PsCreateProcess(programAdress, Priority, ProcessHandle, Args);
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
    ULONG argsLength;
    PCHAR argtmp;

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
    //Allocate space for and copy Args.
    if (Args != NULL) {
        argsLength = RtlStringLength(Args);
        if (argsLength > 0) {
            argtmp = (PCHAR) MmAlloc(argsLength * sizeof (CHAR));
            RtlCopyString(argtmp, Args);
            process->Args = argtmp;
            process->Context.A0 = (ULONG) argtmp;
        } else
            Args = NULL;
    }


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

VOID
PsKillMe() {
    STATUS status;
    status = PsKillProcess(KeCurrentProcess, 0);
    if (0 != status)
        KdPrint("KillMe failed");
}

STATUS
PsKillByPID(
        ULONG PID,
        ULONG ExitStatus
        ) {
    PPROCESS pprocess=NULL;
    STATUS status;

    
    //Get process
    status = PsReferenceProcess(PID, &pprocess);
   
    if (0 != status)
        return status;

    ASSERT(pprocess);

    status = PsKillProcess(pprocess, ExitStatus);
    if (0 != status) {
        ObDereferenceObject(pprocess);
        return status;
    }
    ObDereferenceObject(pprocess);
    return status;
};

STATUS
PsKillProcess(
        PPROCESS PProcess, //remove
        ULONG ExitStatus
        ) {
    STATUS status;

    status = ObReferenceObject(PProcess, processType);
    if (status != 0)
        return status;

    status = KeStopSchedulingProcess(PProcess);
    if (status != 0) return status;
#ifdef DEBUG_PS
    KdPrint("descheduled process");
#endif

    ObKillProcess(PProcess);
    MmFree(PProcess->AllocatedMemory);

#ifdef DEBUG_PS
    KdPrint("PsKillP freed memory");
#endif
    //Free memory allocated for Args
    if (PProcess->Args != NULL)
        MmFree(PProcess->Args);

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
    PPROCESS pprocess;

    pprocess = ObGetFirstObjectOfType(processType);

    while (pprocess) {
        if (pprocess->PID == PID) {
            status = ObOpenObjectByPointer(pprocess, 0, processType, ProcessHandle);
            return status;
        }
        pprocess = ObGetNextObjectOfType(pprocess);
    }
    return STATUS_NO_SUCH_PROCESS;
}

STATUS
PsReferenceProcess(
        ULONG PID,
        PPROCESS *ProcessPtr
        ) {
    PPROCESS pprocess;

    pprocess = ObGetFirstObjectOfType(processType);

    while (pprocess) {
        if (pprocess->PID == PID) {
            *ProcessPtr = pprocess;
            ObReferenceObject(pprocess, processType);
            return STATUS_SUCCESS;
        }
        pprocess = ObGetNextObjectOfType(pprocess);
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

