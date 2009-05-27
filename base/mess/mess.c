#include <arcos.h>
#include <mm.h>
#include <mess.h>
#include <rtl.h>
#include <ps.h>
#include <ke.h>
#include <kd.h>
#include <ob.h>

PMESS_PROCESS_QUEUE processQueue;

STATUS
MessInitialize() {

    // Init process queue.
    processQueue = MmAlloc(sizeof (MESS_PROCESS_QUEUE));
    if (!processQueue)
        return STATUS_NO_MEMORY;
    processQueue->first = NULL;
    processQueue->last = NULL;

    return STATUS_SUCCESS;
}

ULONG
LengthOfQueue(PMESS_PROCESS_NODE node){
    if(node)
        return 1 + LengthOfQueue(node->next);
    else
        return 0;
}

STATUS
AddProcessToQueue(ULONG type) {

    PMESS_PROCESS_NODE newNode;

    // CHECK! DONT ADD TO QUEUE IF ALREADY IN IT!!!
    //KdPrint("length = %d", LengthOfQueue(processQueue->first));

    // Init node.
    newNode = MmAlloc(sizeof (MESS_PROCESS_NODE));
    if (!newNode) return STATUS_NO_MEMORY;
    newNode->process = KeCurrentProcess;
    newNode->next = NULL;
    newNode->pid = KeCurrentProcess->PID;
    newNode->type = type;
    //KdPrint("Adding new node to processQueue. pid = %d", newNode->pid);
    if (!processQueue->first && !processQueue->last) { // No nodes in queue.
        processQueue->first = processQueue->last = newNode;
    } else { // Some node in queue.
        //KdPrint("add");
        processQueue->last->next = newNode; // last should NOT be null if first isn't.
        //KdPrint("add2");
        processQueue->last = newNode;
        //KdPrint("add3");
    }

    return STATUS_SUCCESS;

}

// This function is a bit strange since the loop is supposed to be outside it.

STATUS
RemoveProcessFromQueue(PMESS_PROCESS_NODE iterator, PMESS_PROCESS_NODE iteratorPrev) {

    // Is it the first method in queue?
    if (processQueue->first == iterator && processQueue->last == iterator){
        processQueue->first = processQueue->last = NULL;
        MmFree(iterator);
    }
    else if (processQueue->first == iterator) {
        processQueue->first = processQueue->first->next;
        // Is this the only node?
        if (processQueue->last == iterator) processQueue->last = NULL; // Should work.
        MmFree(iterator);
    }        // Is process last in queue?
    else if (processQueue->last == iterator) {
        processQueue->last = iteratorPrev;
        MmFree(iterator);
    }        // Else in middle... :(
    else {
        iteratorPrev->next = iterator->next;
        MmFree(iterator);
    }

    // Memcheck?
    return STATUS_SUCCESS;
}

// This method is used to delete processNode from processQueue when its done
// waiting and got no message.

VOID
ResumeMethod(PPROCESS process) {

    PMESS_PROCESS_NODE iterator;
    PMESS_PROCESS_NODE iteratorPrev;

    iterator = iteratorPrev = processQueue->first;
    while (iterator) {
        if (iterator->pid == process->PID) {
            RemoveProcessFromQueue(iterator,iteratorPrev);
            //KdPrint("deleted from queue");
        }
        iteratorPrev = iterator;
        iterator = iterator->next;
    }
}

STATUS
MessSendMessage(
        ULONG receiverPid,
        ULONG type,
        PVOID buffer,
        ULONG bufferSize // Is bufferSize in bytes?
        ) {
    PPROCESS pprocess;
    PMESSAGE message;
    PMESSAGE mq;
    STATUS status;
    PMESS_PROCESS_NODE iterator = processQueue->first;
    PMESS_PROCESS_NODE iteratorPrev; // To fix deletion of node in processQueue.

    ASSERT(buffer); // Allowing sending message without buffer?

    //KdPrint("MESS: SendMessage");

    message = (PMESSAGE) MmAlloc(sizeof (MESSAGE) + bufferSize);
    if (message == NULL) // Check out of memory.
        return STATUS_NO_MEMORY;

    // Init mess
    message->senderPid = KeCurrentProcess->PID;
    message->receiverPid = receiverPid;
    message->priority = 1; // NOTE TO SELF: FIX THIS?
    message->type = type;
    message->bufferSize = bufferSize;
    message->next = NULL;
    // Copy input to mess buffer.
    RtlCopyMemory((PCHAR) message + sizeof (MESSAGE), buffer, bufferSize);
    message->buffer = (PCHAR) message + sizeof (MESSAGE);

    // Get receiver process to check its state and pid.
    status = PsReferenceProcess(receiverPid, &pprocess);
    // Check if status exists.
    if (status != 0) {
        //KdPrint("No such process in SendMessage - check pid!");
        return status;
    }
    // Check if the process is waiting, going through processQueue.
    // If the process is in the queue but isn't blocked, it has stopped
    // waiting and should be removed from the queue. The message should
    // still be added to its message queue though.
    iterator = iteratorPrev = processQueue->first;
    while (iterator) {
        if (iterator->pid == receiverPid && iterator->process->State != blocked) {
            // If we arrived here, things are a bit fucked up. The
            // process is in the queue but not blocked.
            // Remove it.
            //KdPrint("should not happen");
            RemoveProcessFromQueue(iterator, iteratorPrev);
        } else if (iterator->pid == receiverPid && iterator->process->State == blocked) {
            //KdPrint("Message found!");
            // Fix result and resume process.
            // Is the process receiving first or is process waiting for same type as the new message?
            if(iterator->type == MESSAGE_TYPE_FIRST || iterator->type == message->type) {
                KeSetSyscallResult(pprocess, (ULONG) message);
                KeResumeProcess(pprocess);
                RemoveProcessFromQueue(iterator,iteratorPrev);
            }
            // If not, the type of message is not what the process is waiting for. Don't do anything.
        }
        iteratorPrev = iterator;
        iterator = iterator->next;
    }


    ASSERT(pprocess);

    // Add message to process' message queue.
    mq = pprocess->MessageQueue;
    if (mq == NULL) pprocess->MessageQueue = message;
    else {// Add message last in queue.
        while (mq) {
            if (mq->next == NULL) {
                mq->next = message;
                // Break hack
                break;
            }
            //KdPrint("mq");
            mq = mq->next;
        }
    }
    ObDereferenceObject(pprocess);

    return STATUS_SUCCESS;
}

PVOID
MessReceiveFirst(
        ULONG timeout
        ) {
    PMESSAGE newMessage;

    //KdPrint("MESS: ReceiveFirst");

    if (KeCurrentProcess->MessageQueue != NULL) // Message queue not empty - check it.
    {
        newMessage = KeCurrentProcess->MessageQueue;
        KeCurrentProcess->MessageQueue = KeCurrentProcess->MessageQueue->next; // Might be NULL.
        newMessage->next = NULL;
        return newMessage; // Return pointer to new message. Should be copied by the process, I guess, and then deleted.
    } else // No message in message queue - wait a while/timeout and add it to queue.
    {
        AddProcessToQueue(MESSAGE_TYPE_FIRST);
        KeSuspendProcess(timeout, ResumeMethod);
        //KdPrint("Process stopped");
    }
    return NULL;
}

PVOID
MessReceiveType(
        ULONG timeout,
        ULONG type
        ) 
{

    PMESSAGE newMessage;
    PMESSAGE mq, mqPrev;

    if (KeCurrentProcess->MessageQueue != NULL) // Message queue not empty - check it.
    {
        // Is it the first message?
        if(KeCurrentProcess->MessageQueue->type == type){
            newMessage = KeCurrentProcess->MessageQueue;
            newMessage->next = NULL;
            KeCurrentProcess->MessageQueue = KeCurrentProcess->MessageQueue->next;
            return newMessage;
        }
        // Else go through message queue.
        mq = mqPrev = KeCurrentProcess->MessageQueue;
        while(mq){
            // This is the message to get.
            if(mq->type == type) {
                newMessage = mq;
                newMessage->next = NULL;
                mqPrev->next = mq->next;
                return newMessage;
            }
            mqPrev = mq;
            mq = mq->next;
        }
        // If come here, the right type of message is not in queue. Add to waiting queue and suspend.
        AddProcessToQueue(type);
        KeSuspendProcess(timeout, ResumeMethod);
    }
    // No message in message queue - wait a while/timeout and add it to queue.
    else
    {
        AddProcessToQueue(type);
        KeSuspendProcess(timeout, ResumeMethod);
    }
    return NULL;
}


/*

Magnus wrote:
I will need a dealloc message queue function that frees every message in the list and then sets the list pointer to null
This is to free memory when a process is killed.

 */

ULONG
MessGetMessageSize(
        PVOID mess
        ) {
    //KdPrint("GetMessageSize");
    return (((PMESSAGE) mess)->bufferSize + sizeof (MESSAGE)); // Might aswell copy all of the message, including the header.
}

/*
 How to use in user program:

    MESSAGE message = ReceiveFirst(2000)    Receive your message
    PMESSAGE myMessage;                     Declare pointer to your new message
    ULONG sizeOfMessage;
    STATUS status;
    sizeOfMessage = GetMessageSize(message); Use this function to get the size of message.
    myMessage = Malloc(sizeOfMessage);  Declare memory to your new message. You have to include the MESSAGE header when calculating the size since you're copying all of the message, just not its buffer.
    status = CopyMessage(myMessage,message,sizeOfMessage);


 */

STATUS
MessCopyMessage(
        PVOID messDest,
        PVOID messSource,
        ULONG sizeOfMessage
        ) {
    //KdPrint("CopyMessage");
    ASSERT(messDest);
    ASSERT(messSource);
    // Check that dest buffer is big enough to contain source message.
    if (sizeOfMessage < ((PMESSAGE) messSource)->bufferSize + sizeof (MESSAGE))
        return STATUS_BUFFER_TOO_SMALL;
    RtlCopyMemory(messDest, messSource, (((PMESSAGE) messSource)->bufferSize) + sizeof (MESSAGE));

    return STATUS_SUCCESS;
}

STATUS
MessDeleteMessage(
        PVOID mess
        ) {
    PMESSAGE mq; // mq = MessageQueue of current process.
    PMESSAGE message;
    PMESSAGE iterator;
    ASSERT(mess);
    ASSERT(KeCurrentProcess);

    mq = iterator = KeCurrentProcess->MessageQueue;
    message = (PMESSAGE) mess;

    if (mq == NULL) {
        //KdPrint("MQ empty");
        return STATUS_SUCCESS; // Hm.
    }

    //unlink message from message list;
    if (mq == message) { // It's the first message
        //KdPrint("Deleting first message");
        KeCurrentProcess->MessageQueue = message->next; // Either NULL or second message in list.
        MmFree(message);
    } else {
        while (iterator) {
            if (iterator->next == message) {
                iterator->next = message->next;
                MmFree(mess);
                //KdPrint("Deleting non-first message");
                return STATUS_SUCCESS;
            } else
                iterator = iterator->next;
        }
    }

    return STATUS_INVALID_HANDLE; // Message is not in queue. The best status I could find.
}

STATUS
MessDeleteMessageQueue(PMESSAGE messageQueue) {
    PMESSAGE mq, message;
    //KdPrint("DeleteMessageQueue");

    mq = messageQueue;

    while (mq) {
        message = mq;
        mq = mq->next;
        MmFree((PVOID) message);
    }

    messageQueue = NULL; // Does this change anything really?

    return STATUS_SUCCESS;
}
