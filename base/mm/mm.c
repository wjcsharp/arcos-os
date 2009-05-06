/*
        Module: mm.c
        Author: Olle Harstedt

*/

#include <arcos.h>      // Which one are necessary here ??
#include <types.h>
#include <ob.h>
#include <ke.h>
#include <mm.h>
#include <hal.h>        // For HalGetFirstUsableMemoryAdress().

PVOID freeMemPointer;  // The pointer to the free adress space.

VOID
MmInitialize()
{
        freeMemPointer = HalGetFirstUsableMemoryAddress();    // Correct? A lot of warnings here...
}

PULONG
MmGetMemPointer(){
        return freeMemPointer;
}

PVOID
MmAlloc(
        ULONG size      // Could we use parameter "type" here instead?
        )
{
        POBJECT_HEADER returnPointer = freeMemPointer;          // Give current address to object
        freeMemPointer = freeMemPointer + size;                 // Increase freeMemPointer	
	return returnPointer;
}

VOID
MmFree(
	POBJECT_HEADER objectHeader
	)
{
	// Add code here.
}
