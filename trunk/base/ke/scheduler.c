/*

Module Name:

    scheduler.c

Abstract:

    ARCOS scheduler.

Author:

    Michal Strehovsky

Revision History:

*/

#include <arcos.h>
#include <ke.h>
#include <ob.h>

//
// Scheduling queues: each priority class has one queue
//
PROCESS_QUEUE KepSchedulingQueues[PROCESS_PRIORITY_LEVELS];

//
// List of processes currently blocked
//
PPROCESS KepBlockedList;

//
// List of processes waiting for a timer to expire
//
PPROCESS KepTimerList;

//
// Holds control block for the currently active process
//
PPROCESS KeCurrentProcess;

VOID
KepEnqueueProcess(
    PPROCESS Process
    )
{
    ASSERT(Process->NextPCB == NULL);
    ASSERT(Process->Priority < PROCESS_PRIORITY_LEVELS);
    
    //
    // is there another process in this queue?
    //
    if (KepSchedulingQueues[Process->Priority].First) {

        ASSERT(KepSchedulingQueues[Process->Priority].Last->NextPCB == NULL);
        
        KepSchedulingQueues[Process->Priority].Last->NextPCB = Process;
        
        KepSchedulingQueues[Process->Priority].Last = Process;
        
    } else {

        KepSchedulingQueues[Process->Priority].First = Process;

        KepSchedulingQueues[Process->Priority].Last = Process;
    
    }

    Process->ProcessStatus = ready;
    
}

VOID
KepDequeueProcess(
    PPROCESS Process
    )
{
    PPROCESS currentProcess;
    
    ASSERT(Process->Priority < PROCESS_PRIORITY_LEVELS);

    currentProcess = KepSchedulingQueues[Process->Priority].First;

    //
    // handle special case when Process is the first in the queue
    //
    if (currentProcess == Process) {

        //
        // (if the process was the only one in the queue, it's NextPCB is NULL
        // and that's enough for us - no need to check for this special case)
        //
        KepSchedulingQueues[Process->Priority].First = currentProcess->NextPCB;

        currentProcess->NextPCB = NULL;

        return;
    }

    //
    // find the process in the queue
    //
    while (currentProcess->NextPCB) {

        if (currentProcess->NextPCB == Process) {

            //
            // unlink process from the list
            //
            currentProcess->NextPCB = Process->NextPCB;

            //
            // handle special case when this process was the last on the list
            //
            if (KepSchedulingQueues[Process->Priority].Last == Process) {

                KepSchedulingQueues[Process->Priority].Last = currentProcess;
            }

            Process->NextPCB = NULL;

            return;
        }

        //
        // move to the next queue entry
        //
        currentProcess = currentProcess->NextPCB;
    }

    KeBugCheck("KepDequeueProcess: process is not in the priority queue");
}

STATUS 
KeStartSchedulingProcess(
    PPROCESS Process
    )
{
    STATUS status;

    ASSERT(Process);

    //
    // we will be holding a pointer to this object, register it with OB
    //
    status = ObReferenceObject(Process, NULL);
    if (status != STATUS_SUCCESS)
        return status;

    //
    // add process to the scheduling queue
    //
    KepEnqueueProcess(Process);

    return STATUS_SUCCESS;
}

BOOL
KepRemoveFromProcessList(
    PPROCESS *ListHead,
    PPROCESS Process
    )
{
    PPROCESS currentProcess;

    //
    // handle special case where Process is the list head
    //
    if (*ListHead == Process) {
        
        *ListHead = Process->NextPCB;
    } else {
            
        currentProcess = *ListHead;

        //
        // find the process in the list
        //
        while (currentProcess->NextPCB) {

            if (currentProcess->NextPCB == Process) {

                //
                // unlink process from the list
                //
                currentProcess->NextPCB = Process->NextPCB;
                
                return TRUE;
            }

            //
            // move to the next list entry
            //
            currentProcess = currentProcess->NextPCB;
        }
    }

    //
    // process not in the list, return false
    //
    return FALSE;
}

STATUS 
KeStopSchedulingProcess(
    PPROCESS Process
    )
{
    ASSERT(Process);

    //
    // determine in which list the process is located
    //
    switch (Process->ProcessStatus) {

        case running:
            // TODO: currently running - what now???
            break;
            
        case ready:
            // in a ready queue
            KepDequeueProcess(Process);
            break;
            
        case blocked:
            // in a blocked list
            if (!KepRemoveFromProcessList(&KepTimerList, Process)
                && !KepRemoveFromProcessList(&KepBlockedList, Process)) {

                KeBugCheck("KeStopSchedulingProcess: process blocked, but in no blocked queue");
            }
            
            break;

        default:
            KeBugCheck("KeStopSchedulingProcess: unhandled process state");
    }

    //
    // object no longer in our queues -> dereference
    //
    ObDereferenceObject(Process);

    return STATUS_SUCCESS;
}

VOID
KeBlockProcess(
    VOID
    )
{
    
}
    
VOID
KeResumeProcess(
    PPROCESS Process
    )
{
    ASSERT(Process);
    ASSERT(Process->ProcessStatus == blocked);

    //
    // remove process from the blocked list
    //
    if (!KepRemoveFromProcessList(&KepTimerList, Process)
        && !KepRemoveFromProcessList(&KepBlockedList, Process)) {

        KeBugCheck("KeResumeProcess: process blocked, but in no blocked queue");
    }

    //
    // add process to the scheduling queue
    //
    KepEnqueueProcess(Process);
}

VOID 
KeChangeProcessPriority(
    PPROCESS Process,
    ULONG NewPriority
    )
{
    ASSERT(Process);
    ASSERT(NewPriority < PROCESS_PRIORITY_LEVELS);
    
    switch (Process->ProcessStatus) {

        case running:
        case blocked:
            Process->Priority = NewPriority;
            break;

        case ready:
            KepDequeueProcess(Process);
            Process->Priority = NewPriority;
            KepEnqueueProcess(Process);
            break;

        default:
            KeBugCheck("KeChangeProcessPriority: unhandled process state");
    }
}

VOID
KeSuspendProcess(
    ULONG Milliseconds
    )
{
}




