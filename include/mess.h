/*

	Stuff for the message routine. Check out the system design
	document for furhter information.

	Author:
		Olle  Härstedt

*/

#ifndef __mess_h__
#define __mess_h__

#include <arcos.h>

typedef struct _MESSAGE {
	ULONG	senderPid;
	ULONG	receiverPid;
	ULONG	priority;
	ULONG	messageType;
	//PVOID	buffer;
	ULONG	bufferSize;
	struct _MESSAGE *next;
} MESSAGE, *PMESSAGE;

// Simple single-linked list for message queue.
typedef struct _MESSAGE_QUEUE
{
	PMESSAGE	message;	// Should it be MESSAGE here?
	PMESSAGE	next;
} MESSAGE_QUEUE, *PMESSAGE_QUEUE;

STATUS
MessSend(
	ULONG senderPid,
	ULONG receiverPid,
	ULONG priority,
	ULONG messageType,
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
MessReceiveByType(
	ULONG timeout,
	ULONG type
	);

ULONG
MessGetSize(
	PMESSAGE mess
	);

PVOID
MessCopyMessage(
	PVOID mess,		// PVOID or PMESSAGE?
	PVOID buffer,
	ULONG bufferSize
	);

STATUS
MessDeleteMessage(
	PVOID mess
	);

#endif
