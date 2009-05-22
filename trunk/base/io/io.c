#include <arcos.h>
#include <io.h>
#include <hal.h>
#include <ob.h>
#include <rtl.h>
#include <ke.h>
#include <kd.h>
#include <mm.h>

POBJECT_TYPE fileType;
PFILE serialFile;
PFILE lcdFile; 

CHAR outputSpace[512];	// Maximum of string to print = 512
PCHAR outputPointer;
ULONG bufferPosition = 0;
ULONG outputLength;
BOOL doneWriting = TRUE;

static FIFO fifo;			// Input buffer, receives from keyboard
PIO_WAITING_QUEUE waitingQueue;		// Waiting queue for writeFile, FILO.
PIO_BUFFER_QUEUE bufferQueue;

// Pre-define some functions.
VOID IoReadSerial();
VOID IoWriteSerial();
VOID IoWriteLcd();

//
// Added by Olle
// Stops adding when buffer is full.

VOID
AddCharToBuffer(CHAR c) {
    if (fifo.length + 1 < FIFO_SIZE) // length + 1
    {
        fifo.buffer[fifo.length] = c;
        fifo.length++;
    }
}

// Added by Olle.
// Returns first char in fifo buffer, which can be NULL.

CHAR
GetFirstCharFromBuffer() {
    CHAR c;
    ULONG i;
    c = fifo.buffer[0];
    for (i = 0; i != fifo.length; i++) // Move all chars one step. Tested, but not supertested.
        fifo.buffer[i] = fifo.buffer[i + 1]; // Could be optimized. Later.
    if (fifo.length > 0) // Decrease fifo length
        fifo.length--;
    return c;
}


/*
STATUS
AddProcessToWaitingQueue(PVOID buffer, ULONG bufferSize){		// Always adding current process.

	PIO_WAITING_NODE newNode = MmAlloc(sizeof(IO_WAITING_NODE));
	
	//KdPrint("AddProcess begin");

	//Check alloc
	if(newNode == NULL)
		return STATUS_NO_MEMORY;

	newNode->process = KeCurrentProcess;

	// First node in queue?
	if(!waitingQueue->first) {
		ASSERT(!waitingQueue->first);
		waitingQueue->first = waitingQueue->last = newNode;
		// Skip copy - already done in WriteFile.
		// Skip suspend - don't have to wait.
	}
	else {
		ASSERT(waitingQueue->last);
		waitingQueue->last->next = newNode;
		waitingQueue->last = newNode;
		// Copy information about stuff to write (buffer).
		RtlCopyString((PCHAR) newNode->buffer, (PCHAR) buffer);
		newNode->bufferSize = bufferSize;
		//KeBlockProcess();	// Send waiting process to block queue in scheduler.
		//			// OBS OBS OBS! I don't use KeResumeProcess anywhere! Why do they keep waking??

	}
		
	//KdPrint("IO: AddProcessToWaitingQueue: SUCCESS");

	return STATUS_SUCCESS;
}
*/
/*
VOID
RemoveProcessFromWaitingQueue(){	// Always remove process first in queue.

	PIO_WAITING_QUEUE nodeToFree = waitingQueue->first;

	ASSERT(waitingQueue->first);
	
	waitingQueue->first = waitingQueue->first->next;		// Could be NULL.
	MmFree(nodeToFree);

	if(waitingQueue->first) { 	// Is another queue in the list? Then copy stuff to outputBuffer and start the printing.
		RtlCopyString(outputBuffer,(PCHAR) waitingQueue->first->buffer);
		doneWriting = FALSE;
		bufferPosition = 0;
		//outputLength = RtlStringLength((PCHAR) waitingQueue->first->buffer);
	}

	//KdPrint("IO: RemoveProcessFromWaitingQueue");

}
*/
STATUS
IoInitialize() {					// Add mem-check to this function.
    OBJECT_TYPE_INITIALIZER typeInitializer;
    STATUS status;
    HANDLE handle;

    fifo.length = 0;

    // Create fileType
    typeInitializer.DumpMethod = NULL; // What is DumpMethod?
    typeInitializer.DeleteMethod = NULL; // Can't delete file types in this version of ARCOS.
    status = ObCreateObjectType('f', &typeInitializer, &fileType); // f331 = file
    HalDisplayString("Create fileType done\n");

    if (status != STATUS_SUCCESS) // Abandon procedure, and OS, I guess.
    {
        HalDisplayString("Error: Create fileType\n");
        return status;
    }
    // Create serialFile and lcdFile
    handle = NULL;
    status = ObCreateObject(fileType, 0, sizeof (FILE), (PVOID) & serialFile);

    if (status == STATUS_SUCCESS) {
        HalDisplayString("Create serialFile done\n");
        serialFile->read = IoReadSerial;
        serialFile->write = IoWriteSerial;
    } else
        return status;

    status = ObCreateObject(fileType, 0, sizeof (FILE), (PVOID) & lcdFile);
    if (status == STATUS_SUCCESS) {
        HalDisplayString("Create lcdFile done\n");
        lcdFile->read = NULL;
        lcdFile->write = IoWriteLcd;
    }

    waitingQueue = MmAlloc(sizeof(IO_WAITING_QUEUE));
    waitingQueue->first = NULL;
    waitingQueue->last = NULL;

    bufferQueue = MmAlloc(sizeof(IO_BUFFER_QUEUE));
    bufferQueue->first = NULL;
    bufferQueue->last = NULL;

    outputPointer = NULL;

    doneWriting = TRUE;

    return status;
}

HANDLE
IoCreateFile(                         // Error-handling in this function?
        ULONG filename
        ) {
    HANDLE handle = NULL;
    STATUS status;

    // Pick right type.
    if (filename == 's') {
        status = ObOpenObjectByPointer(serialFile, OBJ_INHERIT, fileType, &handle);
        return handle;
    }
    if (filename == 'l') {
        status = ObOpenObjectByPointer(lcdFile, OBJ_INHERIT, fileType, &handle);
        return handle;
    }

    return handle; // Return NULL if everything went ga-ga.
}

ULONG
IoWriteFile(
        HANDLE handle,
        PVOID buffer,
        ULONG bufferSize) {
    STATUS status;
    PFILE file;

    status = ObReferenceObjectByHandle(handle, fileType, (PVOID) & file); // How to solve different types here??
    file->write(buffer, bufferSize);	// Add return stuff here!
    ObDereferenceObject(file); //ADDED BY MAGNUS
    return 0; // Return what?
}

// Don't do anything if there doesn't exists a pointer to a read function.

ULONG
IoReadFile(
        HANDLE handle,
        PVOID buffer,
        ULONG bufferSize) {
    STATUS status;
    PFILE file;

    status = ObReferenceObjectByHandle(handle, fileType, (PVOID) & file); // Only serial type have write capability in this OS.
    if (file->read != NULL)
        file->read(buffer, bufferSize);
    ObDereferenceObject(file); //ADDED BY MAGNUS
    return 0;
}

// Write string to serial display.
VOID
IoWriteSerial(
        PVOID buffer,
        ULONG bufferSize) {

	PIO_BUFFER_NODE newNode = MmAlloc(sizeof(IO_BUFFER_NODE));

	KdPrint("IoWriteSerial");

	// Copy buffer to nodes buffer (has to end with null).
	RtlCopyString(newNode->buffer,(PCHAR) buffer);	
	newNode->next = NULL;

	// Insert new node.
	if(bufferQueue->first == NULL) {
		bufferQueue->first = newNode;
		bufferQueue->last = newNode;
		outputPointer = bufferQueue->first->buffer;
	}
	if(bufferQueue->first){
		bufferQueue->last->next = newNode;
		bufferQueue->last = newNode;
	}
}

// Read characters from the Io-buffer.
VOID
IoReadSerial(
        PVOID buffer,
        ULONG bufferSize) // Is bufferSize needed here?
{
    ULONG p; // To get rid of warning.
    CHAR c;
    p = bufferSize;
    /*
            c =
            while(!c){
                    Sleep(100);						// Check number.
                    c = GetFirstCharFromBuffer();
            }
            // The user program should make a loop when
            // waiting for a char from the buffer:
            //  while(!c){
            //		IoReadFile(handle,c,1);
            //  }
     */

    *((CHAR*) buffer) = GetFirstCharFromBuffer();
}

// Write up to 8 characters to the lcd display (NOT the led board), should change name).
// Only writes 8 chars, even if bufferSize > 8.

VOID
IoWriteLcd(
        PVOID buffer,
        ULONG bufferSize) {
    ULONG i;
    PCHAR string = buffer;
    volatile PCHAR lcdAddress;
    lcdAddress = (PCHAR) 0xbf000418; // Address of first char on ascii-board.
    for (i = 0; i < bufferSize && i < 8; i++) {
        *lcdAddress = string[i];
        lcdAddress += 8;
    }
}

VOID
IoInterruptHandler(CHAR c) {
    AddCharToBuffer(c);
}

// Device is ready for ouput - write the next char on console.
VOID
IoTransmitterInterruptHandler() {
	PIO_BUFFER_NODE tempNode;	// Move this?	

	if(outputPointer && *outputPointer)
		HalDisplayChar(*outputPointer++);
	if(outputPointer && *outputPointer == NULL) {				// Done writing.
		KdPrint("*outputPointer == NULL");
		if(bufferQueue->first == bufferQueue->last) {
			KdPrint("only one node in queue");
			MmFree(bufferQueue->first);
			bufferQueue->first = NULL;
			bufferQueue->last = NULL;
			outputPointer = NULL;
		}
		else {
			KdPrint("next == NULL");
			tempNode = bufferQueue->first;
			bufferQueue->first = bufferQueue->first->next;
			outputPointer = bufferQueue->first->buffer;
			MmFree(tempNode);
		}
	
	}
}
