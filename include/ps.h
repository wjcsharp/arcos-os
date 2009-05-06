/* 
 * File:   ps.h
 * Author: Magnus Soderling
 *
 * Created on den 4 maj 2009, 22:44
 */
#ifndef _PS_H
#define	_PS_H

#include <arcos.h>
//#include <mm.h> Generates build error 090506
//#include <io.h> Not implemented


/* PROCESS CONTROL BLOCK
 * PROCESS and datastructures of PROCESS
 */
//Defines possible states of a process.
typedef enum {
    running, ready, waiting, done
}PROCESS_STATUS, *PPROCESS_STATUS;


typedef struct _HANDLE_TABLE_ENTRY {
    ULONG Attributes;
    PVOID Object;
} HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;

//
// specifies the maximum number of open handles in a process
//
#define MAX_HANDLE_COUNT 256

typedef struct _PROCESS {
    CONTEXT Context;
    HANDLE_TABLE_ENTRY HandleTable[MAX_HANDLE_COUNT];
    PROCESS_STATUS ProcessStatus;
    ULONG Priority;
    PVOID ProgramCounter;
    PVOID StackCounter;
    PVOID AllocatedMemory;//Assume only one allocation per process
    ULONG CPUTime;
    // PMESSAGEQUEUE MessageQueue; MessageQUEUE not in any header
    // PMESSAGEQUEUE Supervisor;
    struct _PROCESS *NextPCB;
} PROCESS, *PPROCESS;




#endif	/* _PS_H */

