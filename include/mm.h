#ifndef __MM_H__
#define __MM_H__

#include <arcos.h>
#include <ke.h>

// Main memory for memory manager
#define MAIN_MEM_SIZE 6144000

// Memory block struct
typedef struct _MEMORY_BLOCK {
	ULONG Size;
	BOOL IsFree;
	struct _MEMORY_BLOCK *NextBlock, *PreviousBlock;
} MEMORY_BLOCK, *PMEMORY_BLOCK;

// Virtual block struct
typedef struct _VIRTUAL_MEMORY_BLOCK {
	struct _VIRTUAL_MEMORY_BLOCK *NextBlock;
} VIRTUAL_MEMORY_BLOCK, *PVIRTUAL_MEMORY_BLOCK;


VOID
MmInitialize();         // NEW! Use this in KeInit to initialize   
			// the memory manager (freeMemPointer etc).

// Returns the size of all allocated blocks
ULONG
MmGetUsedMemSum(); 
       
// Prints out all blocks
VOID 
MmPrintBlocks();

PVOID
MmVirtualAlloc(PPROCESS Proc, ULONG Size);

VOID
MmVirtualFree(PPROCESS Proc, PVOID BlockBody);

PVOID
MmAlloc(ULONG Size);

VOID
MmFree(PVOID BlockBody);				

#endif
