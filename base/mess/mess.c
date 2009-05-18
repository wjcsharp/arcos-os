#include <arcos.h>
#include <mm.h>
#include <mess.h>
#include <rtl.h>
#include <ps.h>
#include <ke.h>

STATUS
MessSendMessage(
    ULONG receiverPid,
    ULONG messageType,
    PVOID buffer,
    ULONG bufferSize		// Is bufferSize in bytes?
    )
{
	PHANDLE pprocessHandle;
	PMESSAGE message = (PMESSAGE) MmAlloc(sizeof(MESSAGE) + bufferSize);
	STATUS status;

	ASSERT(buffer);

	message->senderPid = KeCurrentProcess->PID;
	message->receiverPid = receiverPid;
	//message->priority = priority;
	message->priority = 1;		// NOTE TO SELF: FIX THIS!
	message->messageType = messageType;
	message->bufferSize = bufferSize;

    
	RtlCopyMemory((PCHAR)message + sizeof(MESSAGE), buffer, bufferSize);

	//status = PsReferenceProcess(receiverPid,&&process);	// Get receiver process.
	status = PsOpenProcess(receiverPid,&pprocessHandle);
/*
	ASSERT(pprocess);
	pprocess->MessageQueue = message;			// Add message.

*/

	/*****
    if (receiver is waiting for a message) 
	{
        // KeSetSyscallResult is not implemented yet
        KeSetSyscallResult(receiver, (ULONG)message);
        // starts scheduling the process again
        KeResumeProcess(receiver);
    }
	*/

	return STATUS_SUCCESS;
}



PVOID
MessReceiveFirst(
    ULONG timeout
    )
{

	PMESSAGE newMessage;

	if (KeCurrentProcess->MessageQueue != NULL)
	{
		newMessage = KeCurrentProcess->MessageQueue;
		newMessage->next = NULL;		// No tail with messages on new message.
		KeCurrentProcess->MessageQueue = KeCurrentProcess->MessageQueue->next;	// Jump forward one step in the queue, skipping the first one.
		return newMessage;	// Return pointer to new message. Should be copied by the process, I guess, and then deleted.
	}		
	else	// No message in message queue - wait a while/timeout.
	{
		KeSuspendProcess(timeout);
		return NULL;
	}

	//KdPrint("MESS: Message queue in current process is NULL");

	/*
    if (message is available in the msg list) {
        return (PVOID)message;
    }
    remember that this process is waiting for a message;
    // stops scheduling the process until the timeout expires
    // when the timeout expires, NULL will be returned.
    // but if someone sends a message before the timeout expires,
    // KeSetSyscallResult in MessSend will overwite this value before
    // the syscall returns in the resumed process
    return NULL;
	*/
}

 
/*************************

Magnus wrote:
I will need a dealloc message queue function that frees every message in the list and then sets the list pointer to null
This is to free memory when a process is killed.


ULONG

MessGetSize(

    PVOID mess

    )

{

    return ((PMESSAGE)mess)->bufferSize (maybe + sizeof(MESSAGE));

}

 

PVOID

MessCopyMessage(

    PVOID mess,         // PVOID!

    PVOID buffer,

    ULONG bufferSize

    )

{

    PMESSAGE message = (PMESSAGE)mess;

    

    if (message->bufferSize (maybe + sizeof(MESSAGE)) > bufferSize)

        fail;

 

    RtlCopyMemory(buffer, message, size);

    

    MessDeleteMessage(mess);

    

    succeed;

}

 

STATUS

MessDeleteMessage(

    PVOID mess

    )

{

    unlink message from message list;

 

    MmFree(mess);

}






*************************/
