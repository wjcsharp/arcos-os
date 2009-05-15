#include <arcos.h>
#include <io.h>
#include <hal.h>
#include <ob.h>
#include <rtl.h>

POBJECT_TYPE	fileType;
PFILE		serialFile;
PFILE		lcdFile;	// NOT LED! The led-board will not be used, but the LCD-display will.)
static FIFO 	fifo;	

// Pre-define some functions.
VOID IoReadSerial();
VOID IoWriteSerial();
VOID IoWriteLcd();

// Added by Olle
// Stops adding when buffer is full. 
VOID
AddCharToBuffer(CHAR c)
{
	if (fifo.length + 1 < FIFO_SIZE)		// length + 1
	{
		fifo.buffer[fifo.length] = c;
		fifo.length++;
	}
} 

// Added by Olle. 
// Returns first char in fifo buffer, which can be NULL.
CHAR
GetFirstCharFromBuffer()
{
	CHAR c;
	ULONG i;
	c = fifo.buffer[0];
	for (i = 0; i != fifo.length; i++)		// Move all chars one step. Tested, but not supertested.
		fifo.buffer[i] = fifo.buffer[i+1];	// Could be optimized. Later.
	if (fifo.length > 0)				// Decrease fifo length
		fifo.length--;
	return c;
}

STATUS
IoInitialize()
{
   	OBJECT_TYPE_INITIALIZER typeInitializer;
    	STATUS status;
 
	fifo.length = 0;
   
	// Create fileType
    	typeInitializer.DumpMethod = NULL;	// What is DumpMethod?
    	typeInitializer.DeleteMethod = NULL;	// Can't delete file types in this version of ARCOS.
    	status = ObCreateObjectType(0xf331, &typeInitializer, &fileType); // f331 = file
	HalDisplayString("Create fileType done\n");
    
	if (status != STATUS_SUCCESS)	// Abandon procedure, and OS, I guess.
	{
  		HalDisplayString("Error: Create fileType\n"); 
		return status;
	}
	// Create serialFile and lcdFile
	HANDLE handle = NULL;
	
	status = ObCreateObject(fileType, 0, sizeof(FILE), &serialFile);
	
	if (status == STATUS_SUCCESS) 
	{
		HalDisplayString("Create serialFile done\n");
		serialFile->read = IoReadSerial;
		serialFile->write = IoWriteSerial;
	}
	else
		return status;     
	
	status = ObCreateObject(fileType, 0, sizeof(FILE), &lcdFile);
	if (status == STATUS_SUCCESS) 
	{
		HalDisplayString("Create lcdFile done\n");
		lcdFile->read = NULL;
		lcdFile->write = IoWriteLcd;
	}
	
	return status;		
}

HANDLE
IoCreateFile(			// Error-handling in this function?
        ULONG filename
        )
{
	HANDLE handle = NULL;
	STATUS status;

	// Pick right type.
	if (filename == 'serial') 
	{
        	status = ObOpenObjectByPointer(serialFile, OBJ_INHERIT, fileType, &handle);
		return handle;   
	}
	if (filename == 'lcd')
	{
	        status = ObOpenObjectByPointer(lcdFile, OBJ_INHERIT, fileType, &handle);
		return handle;
	}

	return handle;	// Return NULL if everything went ga-ga.
}

ULONG
IoWriteFile(
        HANDLE handle,
        PVOID buffer,
        ULONG bufferSize)
{
	STATUS status;
	PFILE file;

	status = ObReferenceObjectByHandle(handle, fileType, &file); // How to solve different types here??
	file->write(buffer,bufferSize);

        return 0;	// Return what?
}

// Don't do anything if there doesn't exists a pointer to a read function.
ULONG
IoReadFile(
        HANDLE handle,
        PVOID buffer,
        ULONG bufferSize)
{
	STATUS status;
	PFILE file;

	status = ObReferenceObjectByHandle(handle, fileType, &file);	// Only serial type have write capability in this OS.
	if (file->read != NULL)
		file->read(buffer,bufferSize);
	
        return 0;
}

// Write string to serial display.
VOID
IoWriteSerial(
        PVOID buffer,
        ULONG bufferSize)
{
	// Insert multitasking routin here.
	PCHAR string = (PCHAR) buffer;
	HalDisplayString(string);
}

// Read characters from the Io-buffer.
VOID
IoReadSerial(
	PVOID buffer,
	ULONG bufferSize)
{
	*((CHAR*) buffer) = GetFirstCharFromBuffer();
}

// Write up to 8 characters to the lcd display (NOT the led board), should change name).
// Only writes 8 chars, even if bufferSize > 8.
VOID
IoWriteLcd(
        PVOID buffer,
        ULONG bufferSize)
{
	ULONG i;
	PCHAR string = buffer;
	volatile PCHAR lcdAddress; 	
	lcdAddress = 0xbf000418;	// Address of first char on ascii-board.
	for (i = 0; i < bufferSize && i < 8; i++)
	{
		*lcdAddress = string[i];
		lcdAddress += 8;
	}
}

VOID
IoInterruptHandler(CHAR c)
{
	AddCharToBuffer(c);
}