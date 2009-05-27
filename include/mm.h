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


// Initialize the memory manager
VOID
MmInitialize();

// Returns the size of all allocated blocks (headers included)
ULONG
MmGetUsedMemSum(); 
       
// Prints out all blocks
VOID 
MmPrintBlocks();

// Returns the sum of memory allocated by a process (headers included)
ULONG
MmGetUsedVirtMemSum();

// Returns the address to the memory allocated
PVOID
MmVirtualAlloc(PPROCESS BlockOwner, ULONG Size);

// Free the memory of a virtual block
VOID
MmVirtualFree(PPROCESS BlockOwner, PVOID BlockBody);

// Free all blocks allocated by the BlockOwner
VOID
MmVirtualFreeAll(PPROCESS BlockOwner);

// Allocate a memory block of size Size and return the address of the block body
PVOID
MmAlloc(ULONG Size);

// Free the block pointing at BlockBody
VOID
MmFree(PVOID BlockBody);				

#endif
