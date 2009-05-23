#include <arcos.h>
#include <mm.h>
#include <mess.h>
#include <rtl.h>
#include <ps.h>
#include <ke.h>
#include <kd.h>

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

	ASSERT(buffer);		// Allowing sending message without buffer?

	KdPrint("MESS: SendMessage");

	if((message = (PMESSAGE) MmAlloc(sizeof(MESSAGE) + bufferSize)) == NULL)
		return STATUS_NO_MEMORY; 

	// Init mess
	message->senderPid = KeCurrentProcess->PID;
	message->receiverPid = receiverPid;
	message->priority = 1;						// NOTE TO SELF: FIX THIS?
	message->type = type;
	message->bufferSize = bufferSize;

	// THIS DOESNT WORK!!!
	RtlCopyMemory((PCHAR) message + sizeof(MESSAGE), buffer, bufferSize);
	message->buffer = (PCHAR) message + sizeof(MESSAGE);
	//KdPrint("MESS: SendMessage: Printing copied buffer content");
	//KdPrint((PCHAR) (message + sizeof(MESSAGE)));

	status = PsReferenceProcess(receiverPid,&pprocess);	// Get receiver process. CHECK STATUS!

	ASSERT(pprocess);
	pprocess->MessageQueue = message;			// Add message.

	return STATUS_SUCCESS;
}



PVOID
MessReceiveFirst(
    ULONG timeout
    )
{

	KeSuspendProcess(5000);
	return NULL;

	PMESSAGE newMessage;

	KdPrint("MESS: ReceiveFirst");

	if (KeCurrentProcess->MessageQueue != NULL)
	{
		newMessage = KeCurrentProcess->MessageQueue;
		newMessage->next = NULL;		// No tail with messages on new message. BUG???
		KeCurrentProcess->MessageQueue = KeCurrentProcess->MessageQueue->next;	// Jump forward one step in the queue, skipping the first one.
		return newMessage;	// Return pointer to new message. Should be copied by the process, I guess, and then deleted.
	}
	else	// No message in message queue - wait a while/timeout.
	{
		KdPrint("MESS: ReceiveFirst: No message in queue - suspending in %d ms", timeout);
		//KeSuspendProcess(timeout);
		return NULL;
	}
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

	while(mq){
		message = mq;
		mq = mq->next;
		MmFree((PVOID)message);
	}

	return STATUS_SUCCESS;
}
