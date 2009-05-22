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

CHAR outputSpace[512]; // Maximum of string to print = 512
PCHAR outputPointer;
ULONG bufferPosition = 0;
ULONG outputLength;
BOOL doneWriting = TRUE;

static FIFO fifo; // Input buffer, receives from keyboard
PIO_WAITING_QUEUE waitingQueue; // Waiting queue for writeFile, FILO.
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
    if (fifo.length + 1 < FIFO_SIZE) // length + 1 BUGBUGBUG
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
    KdPrint("GetFirstCharFromBuffer");
    c = fifo.buffer[0];
    for (i = 0; i != fifo.length; i++) // Move all chars one step. Tested, but not supertested.
        fifo.buffer[i] = fifo.buffer[i + 1]; // Could be optimized. Later.
    if (fifo.length > 0) // Decrease fifo length
        fifo.length--;
    KdPrint("GetFirstCharFromBuffer DONE:%c", c);
    return c;
}

STATUS
IoInitialize() { // Add mem-check to this function.
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

    waitingQueue = MmAlloc(sizeof (IO_WAITING_QUEUE));
    waitingQueue->first = NULL;
    waitingQueue->last = NULL;

    bufferQueue = MmAlloc(sizeof (IO_BUFFER_QUEUE));
    bufferQueue->first = NULL;
    bufferQueue->last = NULL;

    outputPointer = NULL;

    doneWriting = TRUE;

    return status;
}

HANDLE
IoCreateFile(// Error-handling in this function?
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
    file->write(buffer, bufferSize); // Add return stuff here!
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

    PIO_BUFFER_NODE newNode = MmAlloc(sizeof (IO_BUFFER_NODE));

    //KdPrint("IoWriteSerial");

    // Copy buffer to nodes buffer (has to end with null).
    RtlCopyString(newNode->buffer, (PCHAR) buffer);
    newNode->next = NULL;

    // Insert new node.
    if (bufferQueue->first == NULL) {
        bufferQueue->first = newNode;
        bufferQueue->last = newNode;
        outputPointer = bufferQueue->first->buffer;
    }
    if (bufferQueue->first) {
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

    PIO_WAITING_NODE newNode = MmAlloc(sizeof (IO_WAITING_NODE));

    newNode->pbuffer = buffer;
    newNode->bufferSize = bufferSize;
    newNode->pprocess = KeCurrentProcess;
    newNode->next = NULL;

    if (waitingQueue->first == NULL) {
       // KdPrint("serial empty queue");
        waitingQueue->first = waitingQueue->last = newNode;
        KeStopSchedulingProcess(KeCurrentProcess);
    } else {
        ASSERT(waitingQueue->last);
       // KdPrint("serial non empty queue");
        waitingQueue->last->next = newNode;
        waitingQueue->last = newNode;
        KeStopSchedulingProcess(KeCurrentProcess);
    }
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

    PIO_WAITING_NODE tempNode;

    AddCharToBuffer(c);

    if (waitingQueue->first) {
        *(waitingQueue->first->pbuffer) = GetFirstCharFromBuffer();
        tempNode = waitingQueue->first;
        waitingQueue->first = waitingQueue->first->next;
       // KdPrint("before kestartsched");
        KeStartSchedulingProcess(tempNode->pprocess);
       // KdPrint("after kestartsched");
        KdPrint("%x", tempNode);
        //MmFree(tempNode);
       // KdPrint("after MmFree iointerrupt");
    }
}

// Device is ready for ouput - write the next char on console.

VOID
IoTransmitterInterruptHandler() {
    PIO_BUFFER_NODE tempNode; // Move this?

    if (outputPointer && *outputPointer)
        HalDisplayChar(*outputPointer++);
    if (outputPointer && *outputPointer == NULL) { // Done writing.
        //KdPrint("*outputPointer == NULL");
        if (bufferQueue->first == bufferQueue->last) {
            //  KdPrint("only one node in queue");
            MmFree(bufferQueue->first);
            bufferQueue->first = NULL;
            bufferQueue->last = NULL;
            outputPointer = NULL;
        } else {
            //KdPrint("next == NULL");
            tempNode = bufferQueue->first;
            bufferQueue->first = bufferQueue->first->next;
            outputPointer = bufferQueue->first->buffer;
            MmFree(tempNode);
        }

    }
}
