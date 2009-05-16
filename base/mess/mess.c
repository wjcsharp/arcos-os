#include <arcos.h>
#include <mm.h>
#include <mess.h>

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

	PMESSAGE message = (PMESSAGE) MmAlloc(sizeof(MESSAGE) + bufferSize);

	message->senderPid = senderPid;
	message->receiverPid = receiverPid;
	message->priority = priority;
	message->messageType = messageType;
	message->bufferSize = bufferSize;
    
		// Typecasting to PCHAR because of memory size?
	RtlCopyMemory((PCHAR)message + sizeof(MESSAGE), buffer, bufferSize);

    

	//add message to the receiver's message list;

    
	/*****
    if (receiver is waiting for a message) 
	{
        // KeSetSyscallResult is not implemented yet
        KeSetSyscallResult(receiver, (ULONG)message);
        // starts scheduling the process again
        KeResumeProcess(receiver);
    }
	*/
}
