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
    
	if (status != STATUS_SUCCESS)	// Abandon procedure 
		return status;

	// Create LEDType
    	typeInitializer.DumpMethod = NULL;
    	typeInitializer.DeleteMethod = NULL;	// Can't delete file types? 
    	status = ObCreateObjectType('led', &typeInitializer, &LEDType);
    
	return status;    
}

HANDLE
IoCreateFile(			// Error-handling is this function?
        ULONG filename		// Most of the code is from test.c, CreateFoo
        )
{
    HANDLE handle = NULL;
    STATUS status;
    PFILE file;
    HalDisplayString("In IoCreateFile\n");

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
	HalDisplayString("Before return in IoCreateFile\n");
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

        HalDisplayString("In IoWriteFile\n");

	// How to solve different types here??
	status = ObReferenceObjectByHandle(handle, LEDType, &file);
	file->write(buffer,bufferSize);

        return 0;
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

        HalDisplayString("In IoReadFile\n");

	status = ObReferenceObjectByHandle(handle, serialType, &file);
	if (file->read != NULL)
		file->read(buffer,bufferSize);
	
        return 0;
}

VOID
IoInterruptHandler()
{
	// Store read char in buffer done in hal/mipsl32/init.c
	// Not platform independet, I'm afraid.
}

VOID
IoWriteSerial(
        PVOID buffer,
        ULONG bufferSize)
{
	// Insert multitasking routin here.
	PCHAR string = (PCHAR) buffer;
	HalDisplayString(string);
}

VOID
IoReadSerial(			// Character from the interrupt buffer.
	PVOID buffer,
	ULONG bufferSize)
{
	CHAR temp[100];
	HalDisplayString("In IoReadSerial\n");
	PCHAR b = buffer;

	//*b = HalGetFirstCharFromBuffer();
	*((CHAR*) buffer) = 33;
//	*((CHAR*) buffer) = HalGetFirstCharFromBuffer();
}

VOID
IoWriteLED(
        PVOID buffer,
        ULONG bufferSize)
{
	volatile PCHAR ledAddress; 	
	//volatile PCHAR ledAddress = 0xbf000408;	// LED-stuff.
	//*ledAddress = 0xff;
	ledAddress = 0xbf000418;
	HalDisplayString("In IoWriteLED\n");
	*ledAddress = (CHAR) buffer;
	

	/**
	ledAddress = 0x1f000408;
	RtlFillMemory(ledAddress,(CHAR) 1,1);
	ledAddress = 0x1f000418;
	RtlFillMemory(ledAddress,'A',1);
	**/ 

	//ledAddress = 'A';
	//malta->ledbar.reg = 0xff;
	//malta->asciipos[2].value = 'A';
}
