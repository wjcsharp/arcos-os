/*
        Module: mm.c
        Author: Olle Harstedt

*/

#include <arcos.h>      // Which one are necessary here ??
//#include <ke.h>
#include <mm.h>
#include <hal.h>        // For HalGetFirstUsableMemoryAdress().

PVOID freeMemPointer;  // The pointer to the free adress space.

//
// aligns memory pointer at a 4-byte boundary
//
#define ALIGN_MEMORY(X)     ((PVOID)(((ULONG)(X) + 3) & ~3))

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
        PVOID returnPointer = freeMemPointer;                   // Give current address to object
        freeMemPointer = ALIGN_MEMORY((ULONG)freeMemPointer + size); // Increase freeMemPointer	
	return returnPointer;
}

VOID
MmFree(
	PVOID objectHeader
	)
{
	// Add code here.
}
