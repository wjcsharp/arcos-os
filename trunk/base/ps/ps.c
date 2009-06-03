//
// File:   ps.c
// Author: Magnus Söderling
//
//

#include <arcos.h>
#include <apps.h>
#include <ps.h>
#include <mm.h>
#include <ob.h>
#include <ke.h>
#include <rtl.h>
#include <kd.h>
#include <mess.h>
#include <api.h>//SYSCALLS FOR USERPROGRAMS Process_info struct

//#define DEBUG_PS
//
//List of all available applications in the OS
//
POBJECT_TYPE processType;

APPLICATION PsAvailApps[] = {
    {"tm", AppTaskManager},
    {"TaskManager", AppTaskManager},
    {"Kill", AppKill},
    {"ChangePrio", AppChangePrio},
    {"TestProcess3", AppPSTestProcess3},
    {"taskmanager", AppTaskManager},
    {"kill", AppKill},
    {"changeprio", AppChangePrio},
    {"testprocess3", AppPSTestProcess3},
    {"philosopher", AppPhilosopher},
    {"waiter", AppWaiter},
    {"shell", AppShell},
    {"ring", AppRing},
    {"ringnode", AppRingNode},
    {"startwaiter", AppStartWaiter},
    {"drone", AppDrone}

};


BOOL
PIDInUse(ULONG PID);

ULONG
GetNewPID();

VOID
AppPSTestProcess3() {
    //ULONG i;

    HANDLE handtag;
    //PsCreateProcessByName("taskmanager", 1, &handtag, "   asdfg");
    CreateProcess("taskmanager", 1, &handtag, NULL);
    ObCloseHandle(handtag);

    KdPrint("tp3 I AM:%d", GetProcessId());
    SuperviseProc(2, 3);

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
    process->RunningProgram = "IDLE Process";

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
GetNewPID() {
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

PVOID
GetProgramName(VOID(*Execute)(PCHAR Args)) {
    ULONG index;

    if (NULL == Execute)
        return NULL;

    for (index = 0; index < (sizeof (PsAvailApps) / sizeof (APPLICATION)); ++index) {
        if (PsAvailApps[index].Execute == Execute) {
            return PsAvailApps[index].Name;
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

    //Set process status to created
    process->State = created;

    //Ask mamory manager for a chunk of memory to be attached to the process
    memPointer = MmAlloc(PROCESS_MEMORY_SIZE);
    if (memPointer == NULL) {
        ASSERT(createdProcessObject);
        ObDereferenceObject(createdProcessObject);
        return STATUS_NO_MEMORY;
    }

    //Attach memory block
    process->RunningProgram = GetProgramName(PStartingAddress);
    process->AllocatedMemory = memPointer;
    process->PID = GetNewPID();
    process->Priority = Priority;
    process->CPUTime = 0;
    process->Args = Args;
    process->ExitStatus = 654321; //Runningprocess (CRASH exit status)
    //---Initialize Context what needs to be init?
    (process->Context).Pc = (ULONG) PStartingAddress;
    (process->Context).Sp = (ULONG) ((PCHAR) memPointer + PROCESS_MEMORY_SIZE);

    //Initialize handletable
    status = ObInitProcess(KeCurrentProcess, process);
    if (status != 0) {
        ASSERT(createdProcessObject);
        ObDereferenceObject(createdProcessObject);
        MmFree(memPointer);
        return status;
    }

    //Allocate space for and copy Args.
    if (Args != NULL) {
        argsLength = RtlStringLength(Args);
        if (argsLength > 0) {
            argtmp = (PCHAR) MmAlloc(argsLength * sizeof (CHAR));
            RtlCopyString(argtmp, Args);
            process->Args = argtmp;
            process->Context.A0 = (ULONG) argtmp;
        }
    } else
        Args = NULL;

    //Create Handle to object
    status = ObOpenObjectByPointer(createdProcessObject, 0, processType, ProcessHandle);
    if (status != 0) {
        ASSERT(createdProcessObject);
        ObDereferenceObject(createdProcessObject);
        MmFree(memPointer);
        return status;
    }
    process->State = ready;


    //Schedule process
    status = KeStartSchedulingProcess(process);
    if (status != 0) {
        ASSERT(createdProcessObject);
        ObDereferenceObject(createdProcessObject);
        MmFree(memPointer);
        return status;
    }
    ASSERT(createdProcessObject);
    ObDereferenceObject(createdProcessObject);

    return STATUS_SUCCESS;
}

VOID
PsKillMe() {
    STATUS status;
    status = PsKillProcess(KeCurrentProcess, 0); //Exitstatus 0 == Suicide
    if (0 != status)
        KdPrint("KillMe failed"); //BUGBUGBUG
}

STATUS
PsKillByPID(
        ULONG PID,
        ULONG ExitStatus
        ) {
    PPROCESS pprocess = NULL;
    STATUS status;

    //Get process
    status = PsReferenceProcess(PID, &pprocess);
    if (0 != status)
        return status;

    ASSERT(pprocess);

    status = PsKillProcess(pprocess, ExitStatus);
    if (0 != status) {
        ASSERT(pprocess);
        ObDereferenceObject(pprocess);
        return status;
    }
    ObDereferenceObject(pprocess);
    return status;
};

STATUS
PsKillProcess(
        PPROCESS PProcess,
        ULONG ExitStatus
        ) {
    STATUS status;
    CHAR messstring[10];

    //KdPrint("ps Killing:%d", PProcess->PID); //BUGBUGBUG
    status = ObReferenceObject(PProcess, processType);
    if (status != 0)
        return status;

    status = KeStopSchedulingProcess(PProcess);
    if (status != 0) {
        KdPrint("stop scheduling failed in kill process");
        return status;
    }

    //Check for supervisor and if supervised send message
    if (PProcess->Supervisor) {//PID 0 is idle process, idle cant supervise.
        RtlFormatString(messstring, 10, "%d", PProcess->PID);
        SendMessage(PProcess->Supervisor, 0, messstring, (RtlStringLength(messstring) + 1));
    }

    //Free message-queueBUGBUGBUGBUG
    MessDeleteMessageQueue(PProcess->MessageQueue);

    //Clear handletable
    ObKillProcess(PProcess);

    //Free allocated memory BUGBUGBUG
    MmFree(PProcess->AllocatedMemory);

    //Free memory allocated for Args
    if (PProcess->Args)
        MmFree(PProcess->Args);

    PProcess->ExitStatus = ExitStatus;



    //Signal object finished
    ObSignalObject(PProcess);

    //Dereference process
    ASSERT(PProcess);
    ObDereferenceObject(PProcess);
    return STATUS_SUCCESS;
}

ULONG
PsCopyArgs(
        PCHAR Buffer,
        ULONG Buffersize
        ) {
    CHAR c;
    PCHAR args;
    ULONG count = 0;

    args = KeCurrentProcess->Args;

    if (args == NULL) {
        Buffer[count] = '\0';
        return 0;
    }


    while ((c = args[count]) && (Buffersize > count + 1)) {
        Buffer[count] = c;
        count++;
    }
    Buffer[count] = '\0';
    return count;
}

STATUS
PsChangePriority(
        ULONG PID,
        ULONG NewPriority
        ) {
    PPROCESS pprocess = NULL;
    STATUS status;

    // Check args
    if (PID < 1 || NewPriority > 31)
        return STATUS_INVALID_PARAMETER;

    //Get process
    status = PsReferenceProcess(PID, &pprocess);
    if (0 != status)
        return status;
    ASSERT(pprocess);
    //Change priority
    KeChangeProcessPriority(pprocess, NewPriority);
    return STATUS_SUCCESS;
};

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
    ASSERT(process);
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
    ASSERT(process);
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
    ASSERT(pprocess);
    ObDereferenceObject(pprocess);
    return STATUS_SUCCESS;
}

STATUS
PsGetPid(
        HANDLE Handle,
        PULONG PPid
        ) {
    PPROCESS pprocess;
    STATUS status;

    status = ObReferenceObjectByHandle(Handle, processType, (void**) & pprocess);
    if (status != 0)
        return status;

    *PPid = pprocess->PID;
    ASSERT(pprocess);
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
        PPROCESS * ProcessPtr
        ) {
    PPROCESS pprocess;

    pprocess = ObGetFirstObjectOfType(processType);

    while (pprocess) {
        if (pprocess->PID == PID) {
            *ProcessPtr = pprocess;
            ObReferenceObject(pprocess, processType);
            return STATUS_SUCCESS;
        }
        //KdPrint("PsReferenceProcess");
        ASSERT(pprocess);
        pprocess = ObGetNextObjectOfType(pprocess);
        //KdPrint("PsReferenceProcess after get nex obj");
    }
    *ProcessPtr = NULL;
    return STATUS_NO_SUCH_PROCESS;
}

STATUS
CopyPInfo(
        PPROCESS PProcess,
        PPROCESS_INFO Info) {
    if (NULL == PProcess)
        return STATUS_INVALID_PARAMETER;

    Info->CPUTime = PProcess->CPUTime;
    Info->Priority = PProcess->Priority;
    Info->State = PProcess->State;
    Info->RunningProgram = PProcess->RunningProgram;
    Info->PID = PProcess->PID;
    Info->State = PProcess->State;

    return STATUS_SUCCESS;
};

STATUS
PsGetProcessesInfo(
        PPROCESS_INFO Buffer,
        ULONG BufferSize,
        PULONG NumberProcesses
        ) {
    PROCESS_INFO pinfo;
    PPROCESS pprocess = NULL;
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
        //KdPrint("PsGetProcessesInfo");
        pprocess = ObGetNextObjectOfType(pprocess);
        //KdPrint("PsGetProcessesInfo after get nex obj");
    }
    *NumberProcesses = foundProc;
    return STATUS_SUCCESS;
}

STATUS
PsSupervise(
        ULONG Supervisor,
        ULONG PID
        ) {
    STATUS status;
    PPROCESS pprocess;

    status = PsReferenceProcess(PID, &pprocess);
    if (0 != status)
        return status;

    pprocess->Supervisor = Supervisor;
    KdPrint("PsSuper PID:%d is supervised by %d", pprocess->PID, pprocess->Supervisor);
    ASSERT(pprocess);
    ObDereferenceObject(pprocess);
    return STATUS_SUCCESS;
}

/*
VOID
AppSupervise() {
    ULONG supervPid, ProcessPid;
    PCHAR first, second;

    first = KeCurrentProcess->Args;
    KdPrint(first);

    while ((' ' == *first) && *first)
        ++first;
    KdPrint(first);
    if (first == NULL)
        KdPrint("null in appsupervise");
    KillMe();
}*/