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
#include <mess.h>
#include <api.h>//SYSCALLS FOR USERPROGRAMS

//#define DEBUG_PS
//
//List of all available applications in the OS
//
POBJECT_TYPE processType;

APPLICATION PsAvailApps[] = {
    {"TaskManager", TaskManager},
    {"Kill", Kill},
    {"ChangePrio", ChangePrio},
    {"TestProcess3", PSTestProcess3},
};


BOOL
PIDInUse(ULONG PID);

ULONG
GetPID();

BOOL
appisdigit(CHAR c) {
    return !(c < '0' || c > '9');
}

VOID
ChangePrio() {
    ULONG pid, prio, status;
    PCHAR first, second;
    PCHAR pek = KeCurrentProcess->Args;
    //THE FOLLOWING IS REALLY UGLY so look away!
    if (NULL == pek) {
        KdPrint("ChangePrio needs arguments e.g. ' 2 3'");
        KillMe();
    }
    //remove everything until first digit
    if (!appisdigit(*pek)) {
        while ((!appisdigit(*++pek)) && (*pek != 0));
        if (0 == *pek) {
            KdPrint("ChangePrio needs arguments e.g. ' 2 3'");
            KillMe();
        }
    }
    first = pek; //Start of first stringarg
    //Find end of first arg
    while (appisdigit(*++pek));
    if (0 == *pek) {
        KdPrint("ChangePrio needs 2 arguments e.g. ' 2 3'");
        KillMe();
    }
    *pek = 0; //Set end of first string
    //remove everything until next digit
    while ((!appisdigit(*++pek)) && (*pek != 0));
    if (0 == *pek) {
        KdPrint("ChangePrio needs 2 arguments e.g. ' 2 3'");
        KillMe();
    }
    second = pek; //Start of second stringarg
    //Find end of second arg
    while (appisdigit(*++pek));
    if (0 != *pek)
        *pek = 0;

    pid = RtlAtoUL(first);
    prio = RtlAtoUL(second);

    status = ChangePriority(pid, prio);
    if (0 != status) {
        KdPrint("The pid:%d is not in use", pid);
        KillMe();
    }

    KdPrint("Changed Priority of PID = %s", first);
    KdPrint("Changed priority to: %s", second);
    KillMe();
}

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

ULONG min(ULONG A, ULONG B) {
    if (A < B)
        return A;
    return B;
}

#define TASKM_BUFFER_SIZE 12

VOID
TaskManager() {
    PPROCESS_INFO pinfo;
    ULONG numprocess, i, j, cputmp, tend;
    CHAR strbuff[100];
    HANDLE tmout;
    PCHAR timend[] = {"ms", "s"};

    tmout = CreateFile('s');
    pinfo = (PPROCESS_INFO) MmAlloc(TASKM_BUFFER_SIZE * sizeof (PROCESS_INFO));

    for (j = 0; j < 500; j++) {
        KdPrint("GetProcessInfo BEGIN");
        GetProcessInfo(pinfo, TASKM_BUFFER_SIZE, &numprocess);
        KdPrint("Before WriteFile");
        WriteString(tmout, "---------TASKMANAGER---------\n");
        KdPrint("---taskm DONE");
        //Sleep(1000);
        for (i = 0; i < (min(numprocess, TASKM_BUFFER_SIZE)); i++) {
            if (!pinfo[i].RunningProgram)
                pinfo[i].RunningProgram = "Unnamed";
        }
        for (i = 0; i < numprocess; i++) {
            //Check how big cputime is
            cputmp = pinfo[i].CPUTime;
            if (cputmp < 10000)
                tend = 0;
            else {
                cputmp = cputmp / 1000;
                tend = 1;
            }
            RtlFormatString(strbuff, 100, "%s PID: %d, CPU TIME: %d %s\n", pinfo[i].RunningProgram, pinfo[i].PID, cputmp, timend[tend]);
            KdPrint("Before WriteFile");
            WriteString(tmout, strbuff);
            Sleep(1000);
        }
        Sleep(3000);
    }
    KdPrint("Before WriteFile");
    WriteString(tmout, "--------Task Manager says godbye--------");
    ObCloseHandle(tmout);
    MmFree(pinfo);
    KillMe();
}

VOID
PSTestProcess3() {
    //ULONG i;
    /*
     HANDLE handtag;
     PsCreateProcessByName("TaskManager", 1, &handtag, NULL);
     ObCloseHandle(handtag);
     */
    KdPrint("tp3 I AM:%d", GetProcessId());
    SuperviseProc(2, 1);

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
        ObDereferenceObject(createdProcessObject);
        return STATUS_NO_MEMORY;
    }

    //Attach memory block
    process->RunningProgram = GetProgramName(PStartingAddress);
    process->AllocatedMemory = memPointer;
    process->PID = GetPID();
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
    PPROCESS pprocess = NULL;
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
    return status;
};

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

    //Clear hadletable
    ObKillProcess(PProcess);

    //Free allocated memory BUGBUGBUG
    MmFree(PProcess->AllocatedMemory);

    //Free memory allocated for Args
    if (PProcess->Args != NULL)
        MmFree(PProcess->Args);

    PProcess->ExitStatus = ExitStatus;

    //Free message-queue
    MessDeleteMessageQueue();
    //Dereference process
    ObDereferenceObject(PProcess);
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
    ObDereferenceObject(pprocess);
    return STATUS_SUCCESS;
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
        HANDLE Handle,
        PULONG PPid
        ) {
    PPROCESS pprocess;
    STATUS status;

    status = ObReferenceObjectByHandle(&Handle, processType, (void**) & pprocess);
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
        PPROCESS PProcess,
        PPROCESS_INFO Info) {
    if (NULL == PProcess)
        return STATUS_INVALID_PARAMETER;

    Info->CPUTime = PProcess->CPUTime;
    Info->Priority = PProcess->Priority;
    Info->State = PProcess->State;
    Info->RunningProgram = PProcess->RunningProgram;
    Info->PID = PProcess->PID;

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
    ASSERT(pprocess);

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

