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
AddProcessToQueue(){           // Eventually remove timout
        
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
RemoveProcessFromQueue(){       // Remove?

    ULONG pid;
    
    pid = KeCurrentProcess->PID;
    
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
        PMESSAGE mq;
        STATUS status;
        PMESS_PROCESS_NODE iterator = processQueue->first;
        PMESS_PROCESS_NODE iteratorPrev;        // To fix deletion of node in processQueue.
        PMESS_PROCESS_NODE tempIterator;
        
	ASSERT(buffer);		// Allowing sending message without buffer?

	//KdPrint("MESS: SendMessage");

	message = (PMESSAGE) MmAlloc(sizeof(MESSAGE) + bufferSize);
	if(message == NULL)		// Check out of memory.
		return STATUS_NO_MEMORY; 

	// Init mess
	message->senderPid      = KeCurrentProcess->PID;
	message->receiverPid    = receiverPid;
	message->priority       = 1;						// NOTE TO SELF: FIX THIS?
	message->type           = type;
	message->bufferSize     = bufferSize;
        message->next           = NULL;
	// Copy input to mess buffer.
	RtlCopyMemory((PCHAR) message + sizeof(MESSAGE), buffer, bufferSize);
	message->buffer = (PCHAR) message + sizeof(MESSAGE);

        // Get receiver process to check its state and pid.
       	status = PsReferenceProcess(receiverPid,&pprocess);	
        // Check if status exists.
        if(status != 0) {
            KdPrint("No such process in SendMessage - check pid!");
            return status;
        }
        // Check if the process is waiting, going through processQueue.
        iterator = iteratorPrev = processQueue->first;
        while(iterator){
            if(iterator->pid == receiverPid && iterator->process->State == blocked) {   // I should think about this one more time...
                KdPrint("Message found!");
                // Fix result and resume process.
                KeSetSyscallResult(pprocess, (ULONG) message);
                KeResumeProcess(pprocess);
                // Is process first in queue?
                if(processQueue->first == iterator) {
                    tempIterator = processQueue->first;
                    processQueue->first = processQueue->first->next;
                    MmFree(iterator);
                    break;
                    KdPrint("SendMessage: This should NOT be written! 1");
                }
                // Is process last in queue?
                else if(processQueue->last == iterator){
                    processQueue->last = iteratorPrev;
                    MmFree(iterator);
                    break;
                    KdPrint("SendMessage: This should NOT be written! 2");
                  }
                // Else in middle... :(
                else {
                    iteratorPrev->next = iterator->next;
                    MmFree(iterator);
                    break;
                    KdPrint("SendMessage: This should NOT be written! 3");
                }
            }
            iteratorPrev = iterator;
            iterator = iterator->next;
        }

	ASSERT(pprocess);

        // Add message to process' message queue. 
        mq = pprocess->MessageQueue;
        if(mq == NULL) pprocess->MessageQueue = message;
        // Add message last in queue.
        while(mq) {
            //KdPrint("Wee! I'm looping!");
            if(mq->next == NULL) {
                mq->next = message;
                // Break hack
                mq = NULL;
                break;
                KdPrint("SendMessage: This should never be written");
            }
            mq = mq->next;
        }

        ObDereferenceObject(pprocess);

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

	//KdPrint("MESS: ReceiveFirst");

	if (KeCurrentProcess->MessageQueue != NULL)	// Message queue not empty - check it.
	{
		newMessage = KeCurrentProcess->MessageQueue;		
		KeCurrentProcess->MessageQueue = KeCurrentProcess->MessageQueue->next;	// Might be NULL.
		newMessage->next = NULL;
                return newMessage;	// Return pointer to new message. Should be copied by the process, I guess, and then deleted.
	}
	else	// No message in message queue - wait a while/timeout.
	{
		AddProcessToQueue();
                KeSuspendProcess(timeout);
                //KdPrint("Process stopped");
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
    //KdPrint("GetMessageSize");
    return (((PMESSAGE)mess)->bufferSize + sizeof(MESSAGE)); 	// Might aswell copy all of the message, including the header.
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
    ULONG destBufferSize
    )
{
    //KdPrint("CopyMessage");
    ASSERT(messDest);
    ASSERT(messSource);
    // Check that dest buffer is big enough to contain source message.
    if(destBufferSize < ((PMESSAGE)messSource)->bufferSize)
        return STATUS_BUFFER_TOO_SMALL;
    RtlCopyMemory(messDest,messSource,(((PMESSAGE) messSource)->bufferSize) + sizeof(MESSAGE));

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

	if(mq == NULL) {
            //KdPrint("MQ empty");
            return STATUS_SUCCESS;		// Hm.
        }

	//unlink message from message list;
	if(mq == message){		// It's the first message
                //KdPrint("Deleting first message");
                KeCurrentProcess->MessageQueue = message->next;	// Either NULL or second message in list.
		MmFree(message);
	}
	else{
                while(iterator){
			if(iterator->next == message){
                            iterator->next = message->next;
			    MmFree(mess);
                            //KdPrint("Deleting non-first message");
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
        //KdPrint("DeleteMessageQueue");

	mq = KeCurrentProcess->MessageQueue;

	while(mq){                  // Bug fixed.
		message = mq;
		mq = mq->next;
		MmFree((PVOID)message);
	}

        KeCurrentProcess->MessageQueue = NULL;

	return STATUS_SUCCESS;
}
