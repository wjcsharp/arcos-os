/*
        Module: mm.c
        Author: Hugo Heyman


		A lot of bugfixing and cleaning up has been done.
		I've done some tests and everything so far seems to be working! =)

		
*/

#include <arcos.h> 
#include <mm.h>
#include <hal.h>        // For HalGetFirstUsableMemoryAdress().
#include <kd.h>

// First free memory address
static PVOID FirstMemPointer; 

// Size of the header
static ULONG HeaderSize = sizeof(MEMORY_BLOCK);

// Memory Block list
PMEMORY_BLOCK MemBlock;

// Pointer to the first block
PVOID StartBlock;

//
// aligns memory pointer at a 4-byte boundary
//
#define ALIGN_MEMORY(X)     ((PVOID)(((ULONG)(X) + 3) & ~3))

VOID
MmInitialize()
{
	// Initialize first memory addrress
	FirstMemPointer = ALIGN_MEMORY(HalGetFirstUsableMemoryAddress());

	//Initialize the first block
	MemBlock = FirstMemPointer;
	MemBlock->Size = MAIN_MEM_SIZE - sizeof(MEMORY_BLOCK);
	MemBlock->IsFree = TRUE;
	MemBlock->NextBlock = NULL;
	MemBlock->PreviousBlock = NULL;
	
	// Keeps track of the first block
	StartBlock = MemBlock;
}

PVOID MmAlloc(ULONG SizeToBeAllocated) {
	
	// Address to return
	PVOID ReturnAddress;
	
	// Copy of the mb-list
	PMEMORY_BLOCK PMb = MemBlock;
	
	// Temporary pointers to save block header
	PVOID TempNextBlock;
	ULONG TempSize;
	
	// Search for a free block to use
	while (PMb->NextBlock != NULL) {
		
		// Block big enough and free?
		if(PMb->Size > (ULONG)ALIGN_MEMORY(SizeToBeAllocated + HeaderSize) && PMb->IsFree == TRUE) {
			ULONG FragmentSize = (ULONG)ALIGN_MEMORY(PMb->Size - (SizeToBeAllocated + HeaderSize));
			KdPrint("ketchup!");
			// Is the fragment block bigger than the header?
			if (FragmentSize > HeaderSize) {
				// Return block body
				ReturnAddress = ALIGN_MEMORY((ULONG)PMb + HeaderSize);

				// Set block properties
				PMb->IsFree = FALSE;
				PMb->Size = SizeToBeAllocated;
				TempNextBlock = PMb->NextBlock;
				PMb->NextBlock = ALIGN_MEMORY(PMb + HeaderSize + SizeToBeAllocated);
				PMb->NextBlock->IsFree = TRUE;
				PMb->NextBlock->Size = FragmentSize;
				PMb->NextBlock->NextBlock = TempNextBlock;
				PMb->NextBlock->PreviousBlock = PMb;

				// Is this the first block?
				if(PMb->PreviousBlock == NULL)
					StartBlock = PMb;
				else
					// Point back to the start
					PMb = StartBlock;

				// Save
				MemBlock = PMb;

				return ReturnAddress;

			}

			// ..otherwise alloc the full block (make no fragment block)
			else {
				ReturnAddress = ALIGN_MEMORY(PMb + HeaderSize);
				PMb->IsFree = FALSE;

				// Is this the first block?
				if(PMb->PreviousBlock == NULL)
					StartBlock = PMb;
				else
					// Point back to the start
					PMb = StartBlock;

				// Save
				MemBlock = PMb;

				return ReturnAddress;
			}
		}

		// Check the next block
		else {
			PMb = PMb->NextBlock;
		}
	}
	// Return block body
	ReturnAddress = ALIGN_MEMORY((ULONG)PMb + HeaderSize);

	// Set block properties
	PMb->IsFree = FALSE;
	TempSize = PMb->Size;
	PMb->Size = SizeToBeAllocated;
	TempNextBlock = PMb->NextBlock;
	PMb->NextBlock = ALIGN_MEMORY(PMb + HeaderSize + SizeToBeAllocated);
	PMb->NextBlock->IsFree = TRUE;
	PMb->NextBlock->Size = (ULONG)ALIGN_MEMORY(TempSize - (PMb->Size + (2*HeaderSize)));
	PMb->NextBlock->NextBlock = TempNextBlock;
	PMb->NextBlock->PreviousBlock = PMb;

	// Is this the first block?
	if(PMb->PreviousBlock == NULL)
		StartBlock = PMb;
	else
		// Point back to the start
		PMb = StartBlock;

	// Save
	MemBlock = PMb;

	return ReturnAddress;
}


VOID MmFree(PVOID BlockBody) {
	
	// Find block header
	PVOID PHeader = ALIGN_MEMORY((PCHAR)BlockBody - HeaderSize);
	PMEMORY_BLOCK PMb = PHeader;
	PMb->IsFree = TRUE;
	
	
	// Append next neighbor block?
	if(PMb->NextBlock->IsFree == TRUE) {
		KdPrint("...appending with my next neighbor");
		PMb->Size = (ULONG)ALIGN_MEMORY(PMb->Size + PMb->NextBlock->Size + HeaderSize); 
		PMb->NextBlock = PMb->NextBlock->NextBlock;
		if(PMb->NextBlock != NULL)
			PMb->NextBlock->PreviousBlock = PMb;
	}
	
	
	// Append previous block?
	if(PMb->PreviousBlock != NULL) { 
		if(PMb->PreviousBlock->IsFree == TRUE) {
			KdPrint("...appending with my previous neighbor");
			PMb = PMb->PreviousBlock;
			PMb->Size = (ULONG)ALIGN_MEMORY(PMb->Size + PMb->NextBlock->Size + HeaderSize);
			PMb->NextBlock = PMb->NextBlock->NextBlock;
			if(PMb->NextBlock != NULL)
				PMb->NextBlock->PreviousBlock = PMb;
		}
	}
	
	KdPrint("Now I'm free! (0x%x)", ALIGN_MEMORY((ULONG)PMb + HeaderSize));
	PMb = StartBlock;
	MemBlock = PMb;
	
}
