#ifndef __MM_H__
#define __MM_H__

#include <arcos.h>
#include <ke.h>

// Main memory for memory manager
#define MAIN_MEM_SIZE 6144000

//
// memory block struct
//
typedef struct _MEMORY_BLOCK {
  ULONG Size;
  BOOL IsFree;
  PVOID PreviousUsedBlock;
  struct _MEMORY_BLOCK *NextBlock, *PreviousBlock;
} MEMORY_BLOCK, *PMEMORY_BLOCK;

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
