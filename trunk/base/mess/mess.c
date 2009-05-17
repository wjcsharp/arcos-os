#include <arcos.h>
#include <mm.h>
#include <mess.h>
#include <rtl.h>
#include <ps.h>

STATUS
MessSend(
    ULONG senderPid,
    ULONG receiverPid,
    ULONG priority,
    ULONG messageType,
    PVOID buffer,
    ULONG bufferSize		// Is bufferSize in bytes?
    )
{
	PPROCESS pprocess;
	PMESSAGE message = (PMESSAGE) MmAlloc(sizeof(MESSAGE) + bufferSize);
	STATUS status;

	message->senderPid = senderPid;
	message->receiverPid = receiverPid;
	message->priority = priority;
	message->messageType = messageType;
	message->bufferSize = bufferSize;
    
	// Copy buffer to message->buffer, that is after the message "header"
	// Typecasting to PCHAR because of memory size?
	RtlCopyMemory((PCHAR)message + sizeof(MESSAGE), buffer, bufferSize);

	//add message to the receiver's message list;
	status = PsReferenceProcess(receiverPid,&pprocess);	
	pprocess->MessageQueue = message;

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

/*************************

Magnus wrote:
I will need a dealloc message queue function that frees every message in the list and then sets the list pointer to null
This is to free memory when a process is killed.


PVOID
MessReceiveFirst(
    ULONG timeout
    )
{
    if (message is available in the msg list) {
        return (PVOID)message;
    }
    remember that this process is waiting for a message;
    // stops scheduling the process until the timeout expires
    KeSuspendProcess(timeout);
    // when the timeout expires, NULL will be returned.
    // but if someone sends a message before the timeout expires,
    // KeSetSyscallResult in MessSend will overwite this value before
    // the syscall returns in the resumed process
    return NULL;
}

 

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
