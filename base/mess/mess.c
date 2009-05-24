#include <arcos.h>
#include <mm.h>
#include <mess.h>
#include <rtl.h>
#include <ps.h>
#include <ke.h>
#include <kd.h>

PMESS_PROCESS_QUEUE processQueue;

STATUS
MessInitialize(){

	// Init process queue.
	processQueue = MmAlloc(sizeof(MESS_PROCESS_QUEUE));
	if(!processQueue)
		return STATUS_NO_MEMORY;
	processQueue->first = NULL;
	processQueue->last = NULL;
        
	return STATUS_SUCCESS;
}


STATUS
AddProcessToQueue(ULONG timeout){           // Eventually remove timout
        
	PMESS_PROCESS_NODE newNode;

	// Init node.
	newNode = MmAlloc(sizeof(MESS_PROCESS_NODE));	// Add mem-check.
	newNode->process = KeCurrentProcess;
	newNode->next = NULL;
        newNode->pid = KeCurrentProcess->PID;
        
	if(!processQueue->first && !processQueue->last){	// No nodes in queue.
		processQueue->first = processQueue->last = newNode;		
	}
	else {							// Some node in queue.
		processQueue->last->next = newNode;
		processQueue->last = newNode;
	}
	
        return STATUS_SUCCESS;

}


STATUS
MessSendMessage(
    ULONG receiverPid,
    ULONG type,
    PVOID buffer,
    ULONG bufferSize		// Is bufferSize in bytes?
    )
{
	PPROCESS pprocess;
	PMESSAGE message ;
	STATUS status;
        PMESS_PROCESS_NODE iterator = processQueue->first;

	ASSERT(buffer);		// Allowing sending message without buffer?

	KdPrint("MESS: SendMessage");

	message = (PMESSAGE) MmAlloc(sizeof(MESSAGE) + bufferSize);
	if(message == NULL)		// Check out of memory.
		return STATUS_NO_MEMORY; 

	// Init mess
	message->senderPid = KeCurrentProcess->PID;
	message->receiverPid = receiverPid;
	message->priority = 1;						// NOTE TO SELF: FIX THIS?
	message->type = type;
	message->bufferSize = bufferSize;

       	status = PsReferenceProcess(receiverPid,&pprocess);	// Get receiver process. CHECK STATUS!

        // Check if the process is waiting, going through processQueue.
        
        while(iterator){
            if(iterator->pid == receiverPid) {
                KeSetSyscallResult(pprocess, (ULONG) message);
                KeResumeProcess(pprocess);
                KdPrint("Message found!");
            }
            iterator = iterator->next;
        }
        
	// Copy input to mess buffer.
	RtlCopyMemory((PCHAR) message + sizeof(MESSAGE), buffer, bufferSize);
	message->buffer = (PCHAR) message + sizeof(MESSAGE);


	ASSERT(pprocess);
	pprocess->MessageQueue = message;			// Add message.

	return STATUS_SUCCESS;
}



PVOID
MessReceiveFirst(
    ULONG timeout
    )
{

	//KeSuspendProcess(5000);
	//return NULL;

	PMESSAGE newMessage;

	KdPrint("MESS: ReceiveFirst");

	if (KeCurrentProcess->MessageQueue)	// Message queue not empty - check it.
	{
		newMessage = KeCurrentProcess->MessageQueue;		
		KeCurrentProcess->MessageQueue = KeCurrentProcess->MessageQueue->next;	// Might be NULL.
		newMessage->next = NULL;
                return newMessage;	// Return pointer to new message. Should be copied by the process, I guess, and then deleted.
	}
	else	// No message in message queue - wait a while/timeout.
	{
		AddProcessToQueue(timeout);
                KeSuspendProcess(timeout);
                KdPrint("Process stopped");
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
    )
{
    return (((PMESSAGE)mess)->bufferSize + sizeof(MESSAGE)); 	// Might aswell copy all of the message, including the header.
}



STATUS					// Changed to STATUS.
MessCopyMessage(
    PVOID mess,         // PVOID!
    PVOID buffer,
    ULONG bufferSize
    )
{
    PMESSAGE message = (PMESSAGE)mess;
    if ((message->bufferSize + sizeof(MESSAGE)) > bufferSize)
        return STATUS_BUFFER_TOO_SMALL;
    RtlCopyMemory(buffer, message, bufferSize);
    MessDeleteMessage(mess);
    return STATUS_SUCCESS;
}

STATUS
MessDeleteMessage(
    PVOID mess
    )
{
	PMESSAGE mq;		// mq = MessageQueue of current process.
	PMESSAGE message;
	PMESSAGE iterator;
	ASSERT(mess);
	ASSERT(KeCurrentProcess);

	mq = iterator = KeCurrentProcess->MessageQueue;
	message = (PMESSAGE) mess;

	if(mq == NULL)
		return STATUS_SUCCESS;		// Hm.

	//unlink message from message list;
	if(mq == message){		// It's the first message
		mq = message->next;	// Either NULL or second message in list.
		MmFree(mess);
	}
	else{
		while(iterator){
			if(iterator->next == message){
				iterator->next = message->next;
			    MmFree(mess);
			    return STATUS_SUCCESS;
			}
			else
				iterator = iterator->next;
		}
	}

	return STATUS_INVALID_HANDLE;		// Message is not in queue. The best status I could find.
}

STATUS
MessDeleteMessageQueue()
{
	PMESSAGE mq, message;

	mq = KeCurrentProcess->MessageQueue;

	while(mq){				// BUGBUG!
		message = mq;
		mq = mq->next;
		MmFree((PVOID)message);
	}

	return STATUS_SUCCESS;
}
