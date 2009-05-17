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
#include <rtl.h>

//
// Ready queues: one queue for each priority level
//
PROCESS_QUEUE KepReadyQueues[PROCESS_PRIORITY_LEVELS];

//
// List of processes currently blocked
//
PPROCESS KepBlockedList;

//
// Sorted list of processes waiting for a timer to expire
//
PPROCESS KepTimerList;

//
// Control block of the currently active process
//
PPROCESS KeCurrentProcess;

//
// Number of milliseconds since the system has started
//
ULONG KepTickCount;

///////////////////////////////////////////////////////////////////////////////
//
//  KepEnqueueProcess
//
//      Adds process to one of the ready queues.
//
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
    if (KepReadyQueues[Process->Priority].First) {

        ASSERT(KepReadyQueues[Process->Priority].Last->NextPCB == NULL);
        
        KepReadyQueues[Process->Priority].Last->NextPCB = Process;
        
        KepReadyQueues[Process->Priority].Last = Process;
        
    } else {

        KepReadyQueues[Process->Priority].First = Process;

        KepReadyQueues[Process->Priority].Last = Process;
    
    }

    Process->State = ready;
    
}

///////////////////////////////////////////////////////////////////////////////
//
//  KepDequeueProcess
//
//      Removes a process from the ready queue.
//
VOID
KepDequeueProcess(
    PPROCESS Process
    )
{
    PPROCESS currentProcess;
    
    ASSERT(Process->Priority < PROCESS_PRIORITY_LEVELS);

    currentProcess = KepReadyQueues[Process->Priority].First;

    //
    // handle special case when Process is the first in the queue
    //
    if (currentProcess == Process) {

        //
        // (if the process was the only one in the queue, it's NextPCB is NULL
        // and that's enough for us - no need to check for this special case)
        //
        KepReadyQueues[Process->Priority].First = currentProcess->NextPCB;

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
            if (KepReadyQueues[Process->Priority].Last == Process) {

                KepReadyQueues[Process->Priority].Last = currentProcess;
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

VOID
KepReschedule(
    VOID
    )
{
    LONG i;

    //
    // look for a process with higher priority than the current one
    // (but if current process got blocked, don't limit the search)
    //
    LONG bottomPriority = (KeCurrentProcess->State == blocked) ? 0 : (LONG)KeCurrentProcess->Priority;
    
    //
    // scan the ready queue and find the first non-empty slot
    //
    for (i = PROCESS_PRIORITY_LEVELS - 1; i >= bottomPriority; i--) {

        //
        // is the queue non-empty?
        //
        if (KepReadyQueues[i].First) {

            //
            // if the current process is not blocked, add it to ready queue
            // (if it's blocked, someone else already put it where it belongs)
            //
            if (KeCurrentProcess->State != blocked) {
                KepEnqueueProcess(KeCurrentProcess);
            }

            //
            // lets schedule the first process in this queue
            //
            KeCurrentProcess = KepReadyQueues[i].First;
            KepDequeueProcess(KeCurrentProcess);
            break;
        }
    }

    //
    // if no process could be scheduled, someone killed process 0 - too bad!
    //
    if (KeCurrentProcess->State == blocked)
        KeBugCheck("Critical system process terminated or blocked unexpectedly");

    //
    // reset quantum for this process
    //
    KeCurrentProcess->Quantum = 5;

    //
    // set state to running
    //
    KeCurrentProcess->State = running;
}

///////////////////////////////////////////////////////////////////////////////
//
//  KeStartSchedulingProcess
//
//      Starts scheduling a new process.
//
STATUS 
KeStartSchedulingProcess(
    PPROCESS Process
    )
{
    STATUS status;

    ASSERT(Process);
    ASSERT(Process->NextPCB == NULL);

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

///////////////////////////////////////////////////////////////////////////////
//
//  KepRemoveFromProcessList
//
//      Removes process from the specified list.
//      Returns TRUE if process was on the list, FALSE otherwise.
//
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

                //
                // make sure is't completely unlinked :)
                //
                currentProcess->NextPCB = NULL;
                
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

///////////////////////////////////////////////////////////////////////////////
//
//  KeStopSchedulingProcess
//
//      Stops scheduling specified process.
//
STATUS 
KeStopSchedulingProcess(
    PPROCESS Process
    )
{
    ASSERT(Process);

    //
    // determine in which list the process is located
    //
    switch (Process->State) {

        case running:
            //
            // mark as blocked so that it won't get scheduled again
            //
            Process->State = blocked;

            //
            // choose a different process to run
            //
            KepReschedule();
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

///////////////////////////////////////////////////////////////////////////////
//
//  KeBlockProcess
//
//      Moves current process to the blocked process list and changes it's
//      state to blocked.
//
VOID
KeBlockProcess(
    VOID
    )
{
    KeCurrentProcess->State = blocked;

    //
    // add process to the blocked list
    // 
    KeCurrentProcess->NextPCB = KepBlockedList;
    KepBlockedList = KeCurrentProcess;

    //
    // schedule another process
    //
    KepReschedule();
}

///////////////////////////////////////////////////////////////////////////////
//
//  KeResumeProcess
//
//      Moves process from the blocked or sleeping list to the ready queue
//      and changes it's state to ready.
//
VOID
KeResumeProcess(
    PPROCESS Process
    )
{
    ASSERT(Process);
    ASSERT(Process->State == blocked);

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

    //
    // reschedule if this process has higher priority than the current process
    //
    if (Process->Priority > KeCurrentProcess->Priority) {
        KepReschedule();
    }   
}

///////////////////////////////////////////////////////////////////////////////
//
//  KeChangeProcessPriority
//
//      Changes the priority of specified process.
//
VOID 
KeChangeProcessPriority(
    PPROCESS Process,
    ULONG NewPriority
    )
{
    ASSERT(Process);
    ASSERT(NewPriority < PROCESS_PRIORITY_LEVELS);
    
    switch (Process->State) {

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

///////////////////////////////////////////////////////////////////////////////
//
//  KepWakeUpSleepers
//
//      Scans the list of sleeping processes and wakes up processes whose sleep 
//      timer has expired.
//
VOID
KepWakeUpSleepers(
    VOID
    )
{
    while (KepTimerList && (KepTimerList->WakeUpTime < KepTickCount)) {
        KeResumeProcess(KepTimerList);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  KeSuspendProcess
//
//      Suspends the execution of current process for a the specified time
//      interval.
//
VOID
KeSuspendProcess(
    ULONG Milliseconds
    )
{
    ASSERT(KeCurrentProcess->NextPCB == NULL);
    
    KeCurrentProcess->WakeUpTime = KepTickCount + Milliseconds;

    KeCurrentProcess->State = blocked;

    //
    // add this process to the (sorted) timer list
    //
    if (KepTimerList == NULL || (KepTimerList->WakeUpTime > KeCurrentProcess->WakeUpTime)) {

        KeCurrentProcess->NextPCB = KepTimerList;
        KepTimerList = KeCurrentProcess;
    }
    else {

        PPROCESS current = KepTimerList;

        //
        // find our spot in the list
        //
        while (current->NextPCB && (current->NextPCB->WakeUpTime < KeCurrentProcess->WakeUpTime)) {
            current = current->NextPCB;
        }

        //
        // insert into list
        //
        KeCurrentProcess->NextPCB = current->NextPCB;
        current->NextPCB = KeCurrentProcess;
    }

    KepReschedule();
}

///////////////////////////////////////////////////////////////////////////////
//
//  KeGetTickCount
//
//      Returns the number of milliseconds that have elapsed since the system
//      was started, up to 49.7 days.
//
ULONG
KeGetTickCount(
    VOID
    )
{
    return KepTickCount;
}

///////////////////////////////////////////////////////////////////////////////
//
//  KeCaptureContext
//
//      Stores context passed as a parameter as current process's context.
//
VOID
KeCaptureContext(
    PCONTEXT context
    )
{
    ASSERT(KeCurrentProcess);
    ASSERT(context);
        
    RtlCopyMemory(&KeCurrentProcess->Context, context, sizeof(CONTEXT));
}

///////////////////////////////////////////////////////////////////////////////
//
//  KeRestoreContext
//
//      Copies context of the current process to specified buffer.
//
VOID
KeRestoreContext(
    PCONTEXT context
    )
{
    ASSERT(KeCurrentProcess);
    ASSERT(context);
    
    RtlCopyMemory(context, &KeCurrentProcess->Context, sizeof(CONTEXT));
}

///////////////////////////////////////////////////////////////////////////////
//
//  KeHandleTimer
//
//      Handles timer interrupts.
//
VOID
KeHandleTimer(
    VOID
    )
{
    ASSERT(KeCurrentProcess);
    ASSERT(KeCurrentProcess->Quantum > 0);

    //
    // increment tick counter
    //
    // TODO: use hal to determine timer frequency?
    KepTickCount += 10;
    KeCurrentProcess->CPUTime += 10;

    //
    // wake up any sleeping processes that need a wakeup
    //
    KepWakeUpSleepers();

    //
    // decrement quantum for current process
    //
    KeCurrentProcess->Quantum--;

    //
    // if allocated time expired, reschedule
    //
    if (KeCurrentProcess->Quantum == 0) {
        KepReschedule();
    }


}

