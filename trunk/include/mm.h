#ifndef __MM_H__
#define __MM_H__

#include <arcos.h>
#include <ke.h>
     
VOID
MmInitialize();         // NEW! Use this in KeInit to initialize   
			// the memory manager (freeMemPointer etc).

PULONG
MmGetMemPointer();	// NEW! Return freeMemPointer.
                       
PVOID
MmAlloc(ULONG size);	// CHANGED! Function now return POBJECT_HEADER
			// as used in ObCreateObject in ob.c. See
			// ob.h for definition of _OBJECT_HEADER.

VOID
MmFree(
	PVOID m
	);				
					// CHANGED PARAMETER!
					// Before: PBLOCK
					// Now: Pointer to beginning of objects memory address
#endif
