/*

	Stuff for the message routine. Check out the system design
	document for furhter information.

	Author:
		Olle  H�rstedt

*/

#ifndef __mess_h__
#define __mess_h__

#include <arcos.h>



// Note to anyone: MESS_PROCESS_NODE and MESS_PROCESS_QUEUE are defined
// in arcos.h. 

STATUS
MessInitialize();

STATUS
MessSendMessage(
	ULONG receiverPid,
	ULONG type,
	PVOID buffer,
	ULONG bufferSize
	);

PVOID
MessReceiveFirst(
	ULONG timeout
	);

PVOID
MessReceiveHighestPriority(
	ULONG timeout
	);

PVOID
MessReceiveType(
	ULONG timeout,
	ULONG type
	);

ULONG
MessGetMessageSize(
	PVOID mess
	);

STATUS
MessCopyMessage(
	PVOID mess,
	PVOID buffer,
	ULONG bufferSize
	);

STATUS
MessDeleteMessage(
	PVOID mess
	);

STATUS
MessDeleteMessageQueue(PMESSAGE messageQueue);

#endif
