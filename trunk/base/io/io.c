#include <arcos.h>
#include <io.h>
#include <hal.h>
#include <ob.h>
#include <rtl.h>

POBJECT_TYPE    serialType;
POBJECT_TYPE    LEDType;

// Pre-define some functions.
VOID IoReadSerial();
VOID IoWriteSerial();
VOID IoWriteLED();

STATUS
IoInitialize()
{
   	OBJECT_TYPE_INITIALIZER typeInitializer;
    	STATUS status;
    
	// Create serialType
    	typeInitializer.DumpMethod = NULL;	// What is DumpMethod?
    	typeInitializer.DeleteMethod = NULL;	// Can't delete file types in this version of ARCOS.
    	status = ObCreateObjectType('serial', &typeInitializer, &serialType);
    
	if (status != STATUS_SUCCESS)	// Abandon procedure, and OS, I guess.
		return status;

	// Create LEDType
    	typeInitializer.DumpMethod = NULL;
    	typeInitializer.DeleteMethod = NULL;	// Can't delete file types? 
    	status = ObCreateObjectType('led', &typeInitializer, &LEDType);
    
	return status;    
}

HANDLE
IoCreateFile(			// Error-handling in this function?
        ULONG filename		// Most of the code is from test.c, CreateFoo
        )
{
    HANDLE handle = NULL;
    STATUS status;
    PFILE file;

    // Pick right type.
    if (filename == 'serial') 
    {
      status = ObCreateObject(serialType, 0, sizeof(FILE), &file);
      if (status == STATUS_SUCCESS) 
      {
	file->read = IoReadSerial;
	file->write = IoWriteSerial;
        status = ObOpenObjectByPointer(file, OBJ_INHERIT, serialType, &handle);		// Note to self: This line kind of freaked out when '&handle' was 'handle'.
        ObDereferenceObject(file);
	return handle;
      }        
    }
    if (filename == 'led')
    {
      status = ObCreateObject(LEDType, 0, sizeof(FILE), &file);
      if (status == STATUS_SUCCESS) 
      {
	file->read = NULL;
	file->write = IoWriteLED;
        status = ObOpenObjectByPointer(file, OBJ_INHERIT, LEDType, &handle);
        ObDereferenceObject(file);
	return handle;
      }  
    }

    return handle;		// Return NULL if everything went ga-ga.
}

ULONG
IoWriteFile(
        HANDLE handle,
        PVOID buffer,
        ULONG bufferSize)
{
	STATUS status;
	PFILE file;

	status = ObReferenceObjectByHandle(handle, LEDType, &file); // How to solve different types here??
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

	status = ObReferenceObjectByHandle(handle, serialType, &file);	// Only serial type have write capability in this OS.
	if (file->read != NULL)
		file->read(buffer,bufferSize);
	
        return 0;
}

// Nothing is in here. The fifo-buffer must be in hal/mipsl32/init.c
VOID
IoInterruptHandler()
{
	// Store read char in buffer done in hal/mipsl32/init.c
	// Not platform independet, I'm afraid.
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
	CHAR temp[100];
	PCHAR b = buffer;
	*((CHAR*) buffer) = HalGetFirstCharFromBuffer();	// The fifo-buffer in hal/mipsl32/halp.h
}

// Write up to 8 characters to the ascii-board (NOT the led board), should change name).
// Only writes 8 chars, even if bufferSize > 8.
VOID
IoWriteLED(
        PVOID buffer,
        ULONG bufferSize)
{
	ULONG i;
	PCHAR string = buffer;
	volatile PCHAR ledAddress; 	
	ledAddress = 0xbf000418;	// Address of first char on ascii-board.
	for (i = 0; i < bufferSize && i < 8; i++)
	{
		*ledAddress = string[i];
		ledAddress += 8;
	}
}
