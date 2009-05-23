/*

	Stuff for the message routine. Check out the system design
	document for furhter information.

	Author:
		Olle  Härstedt

*/

#ifndef __mess_h__
#define __mess_h__

#include <arcos.h>

// Define types for messages
#define MESSAGE_TYPE_STRING	0


typedef struct _MESSAGE {
	ULONG	senderPid;
	ULONG	receiverPid;
	ULONG	priority;
	ULONG	type;
	PVOID	buffer;		// Points to body of message
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
MessReceiveByType(
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
MessDeleteMessageQueue();

#endif
