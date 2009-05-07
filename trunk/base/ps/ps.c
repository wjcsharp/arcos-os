/* 
 * File:   ps.c
 * Author: Magnus Söderling
 *
 */

#include <arcos.h>
#include <ps.h>
#include <mm.h>
#include <ob.h>


STATUS
PsCreateProcess(
        PVOID (*PStartingAddress)(),
        ULONG Priority,
        PHANDLE ProcessHandle,
        PCHAR Args
        ){
    STATUS status;
    PVOID mempointer;
    PPROCESS newprocess = (PPROCESS)ProcessHandle;


//Ask object manager for a new processobject
    status = ObInitProcess(NULL, newprocess);
    if (status != STATUS_SUCCESS)
        return status;


    newprocess->Priority = Priority;

    
//Ask mamory manager for a chunk of memory to be attached to the process
    mempointer = MmAlloc(PROCESS_MEMORY_TO_ALLOCATE);
    if (mempointer == NULL)
        return STATUS_NO_MEMORY;
  
    ProcessHandle = (PHANDLE)newprocess;
    return STATUS_SUCCESS;
}