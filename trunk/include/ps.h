/*
 * File:   ps.h
 * Author: Magnus Soderling
 *
 * Created on den 4 maj 2009, 22:44
 */

#ifndef __PS_H__
#define	__PS_H__

#include <arcos.h>
/* Following includes generates build error
#include <ob.h>
#include <mm.h>
*/


/* PROCESS CONTROL BLOCK
 * PROCESS and datastructures of PROCESS
 */

/* Defines possible states of a process.*/
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
    /* PMESSAGEQUEUE MessageQueue; MessageQUEUE not in any header */
    /* PMESSAGEQUEUE Supervisor; */
    struct _PROCESS *NextPCB;
} PROCESS, *PPROCESS;

#endif	// __PS_H__ 

