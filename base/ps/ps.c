/* 
 * File:   ps.c
 * Author: Magnus Söderling
 *
 */

#include <arcos.h>
#include <ps.h>
#include <mm.h>
#include <ob.h>


//Create Process object type

STATUS
CreateProcessObjectType(
        POBJECT_TYPE ProcessType
        ) {
    OBJECT_TYPE_INITIALIZER typeInitializer;
    STATUS status;

    //
    // create a new object type: Process
    //
    typeInitializer.DumpMethod = NULL; //Should be implemented...
    typeInitializer.DeleteMethod = NULL;
    status = ObCreateObjectType('proc', &typeInitializer, &ProcessType);

    return status;
}



STATUS
PsCreateProcess(
        PVOID (*PStartingAddress)(),
        ULONG Priority,
        PHANDLE ProcessHandle,
        PCHAR Args
        )
{
    STATUS status;
    PVOID createdProcessObject=NULL;
    PPROCESS process;
    ///-------SHOULD Be moved to initialize only once
    POBJECT_TYPE processType = NULL;
    status = CreateProcessObjectType(processType);
    //----------------------------------------------------

    status = ObCreateObject(processType, 0, sizeof(PROCESS), createdProcessObject);
    process = (PPROCESS)createdProcessObject;
    process->Priority = Priority;
    process->CPUTime = 0;
    
    return status;
}




/*
//Ask object manager for a new processobject






//Ask mamory manager for a chunk of memory to be attached to the process
mempointer = MmAlloc(PROCESS_MEMORY_TO_ALLOCATE);
if (mempointer == NULL)
    return STATUS_NO_MEMORY;


return STATUS_SUCCESS;



*/