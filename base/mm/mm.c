/*
        Module: mm.c
        Author: Hugo Heyman


		I've done something I should have done from the beginning
		I've put all blocks free or not in the same list and rewritten both MmAlloc and MmFree.
		The code is much easier now at the cost of the performance. There are still bugs to be fixed
		but it's working partially for now. More commenting is allso needed

		
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
		if(PMb->Size > SizeToBeAllocated + HeaderSize && PMb->IsFree == TRUE) {
			ULONG FragmentSize = PMb->Size - (SizeToBeAllocated + HeaderSize);

			// Is the fragment block bigger than the header?
			if (FragmentSize > HeaderSize) {
				
				ReturnAddress = PMb + HeaderSize;
				PMb->IsFree = FALSE;
				PMb->Size = SizeToBeAllocated;
				TempNextBlock = PMb->NextBlock;
				PMb->NextBlock = PMb + HeaderSize + SizeToBeAllocated;
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
				
				ReturnAddress = PMb + HeaderSize;
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
	
	ReturnAddress = ALIGN_MEMORY((ULONG)PMb + HeaderSize);
	PMb->IsFree = FALSE;
	TempSize = PMb->Size;
	PMb->Size = SizeToBeAllocated;
	TempNextBlock = PMb->NextBlock;
	PMb->NextBlock = PMb + HeaderSize + SizeToBeAllocated;
	PMb->NextBlock->IsFree = TRUE;
	PMb->NextBlock->Size = TempSize - (PMb->Size + (2*HeaderSize));
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
	
	PVOID PHeader = (PCHAR)BlockBody - HeaderSize;
	PMEMORY_BLOCK PMb = PHeader;
	PMb->IsFree = TRUE;
	
	KdPrint("Now I'm free!");
	// Append neighbor block?
	if(PMb->NextBlock->IsFree == TRUE) {
		KdPrint("...appending with my next neighbor");
		PMb->Size += PMb->NextBlock->Size + HeaderSize;
		PMb->NextBlock = PMb->NextBlock->NextBlock;
		if(PMb->NextBlock != NULL)
			PMb->NextBlock->PreviousBlock = PMb;
	}
	
	
	
	if(PMb->PreviousBlock != NULL) { 
		if(PMb->PreviousBlock->IsFree == TRUE) {
			KdPrint("...appending with my previous neighbor");
			PMb->Size += PMb->NextBlock->Size + HeaderSize;
			PMb->NextBlock = PMb->NextBlock->NextBlock;
			PMb->NextBlock->PreviousBlock = PMb;
		}
	}
	
	
	PMb = StartBlock;
	MemBlock = PMb;
	/*
	PMEMORY_BLOCK PMb = (PCHAR)BlockBody - HeaderSize;
	if(PMb->NextBlock != NULL){
		PMb->NextBlock->PreviousBlock = PMb;
		KdPrint("asfränt!");
	}

	if(PMb->PreviousBlock != NULL){
		PMb->PreviousBlock->NextBlock = PMb;
		PMb = StartBlock;
		KdPrint("wooh!");
	}
	else
		StartBlock = PMb;
	
	MemBlock = PMb;
	*/
}

/*
VOID MmFree(PVOID BlockBody) {

	// Create a copy of MemBlock
	PMEMORY_BLOCK PMb = MemBlock;

	// Find block header
	PVOID BlockHeader = (PCHAR)BlockBody - HeaderSize;
	PMb = BlockHeader;

	// Add the block to the free ones
	PMb->NextBlock->PreviousBlock = PMb;

	KdPrint("tjohooo");

	// Append neighbor block?
	if(PMb + PMb->Size + HeaderSize == PMb->NextBlock) {
		PMb->Size += PMb->NextBlock->Size + HeaderSize;
		PMb->NextBlock = PMb->NextBlock->NextBlock;
	}

	if(PMb->PreviousBlock == NULL) {

		// This is the start block since the previous is NULL
		StartBlock = PMb;
		
		// Save
		MemBlock = PMb;
	}
	else
	{
		// Add the block to the free ones
		PMb->NextBlock = PMb->PreviousBlock->NextBlock;
		PMb->PreviousBlock->NextBlock = PMb;

		// Save
		MemBlock = StartBlock;
	}	
}
*/