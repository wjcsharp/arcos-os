/*
        Module: mm.c
        Author: Hugo Heyman


		A new pointer "PreviousUsedBlock" was added to the struct and some changes in MmAlloc and MmFree
		Neither MmAlloc or MmFree is fully functioning but I think mallocing a few blocks, 
		then removing the last block and then adding one again is a working case now :P
		
*/

#include <arcos.h> 
#include <mm.h>
#include <hal.h>        // For HalGetFirstUsableMemoryAdress().


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
	MemBlock->PreviousUsedBlock = NULL;
	MemBlock->NextBlock = NULL;
	MemBlock->PreviousBlock = NULL;

	StartBlock = MemBlock;
}


/*
PULONG
MmGetMemPointer(){
        return freeMemPointer;
}
*/

PVOID MmAlloc(ULONG SizeToBeAllocated) {
	
	// Address to return
	PVOID ReturnAddress;
	
	// Copy of the mb-list
	PMEMORY_BLOCK PMb = MemBlock;
	
	// A temporary pointer to save block header
	PVOID TempAllocatedBlock;
	PVOID TempNextBlock;
	ULONG TempSize;
	
	// Search for a free block to use
	while (PMb->NextBlock != NULL) {

		// Block big enough and free?
		if(PMb->Size > SizeToBeAllocated + HeaderSize) {
			ULONG FragmentSize = PMb->Size - (SizeToBeAllocated + HeaderSize);

			// Is the fragment block bigger than the header?
			if (FragmentSize > HeaderSize) {
				
				// Saving "old" header fro block to be allocated
				TempAllocatedBlock = PMb;
				TempNextBlock = PMb->NextBlock;
				PMb->NextBlock = ALIGN_MEMORY((ULONG)PMb + SizeToBeAllocated + HeaderSize);
				PMb->Size = SizeToBeAllocated;
				
				// Set return address
				ReturnAddress = PMb + HeaderSize;

				// Move PMb forward
				PMb = PMb->NextBlock;

				// Keep track of used block header
				PMb->PreviousUsedBlock = TempAllocatedBlock;

				// Set properties of the new block
				PMb->NextBlock = TempNextBlock;
				PMb->Size = FragmentSize;


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
				
				// Save header
				PMb->NextBlock->PreviousUsedBlock = PMb;

				// Remove block from list
				PMb->NextBlock->PreviousBlock = PMb->PreviousBlock;
				PMb->PreviousBlock->NextBlock = PMb->NextBlock;
				
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
	
	// Saving "old" header for block to be allocated
	TempSize = PMb->Size;
	TempAllocatedBlock = PMb;
	PMb->NextBlock = ALIGN_MEMORY((ULONG)PMb + SizeToBeAllocated + HeaderSize);
	PMb->Size = SizeToBeAllocated;

	// Set addres to return
	ReturnAddress = PMb + HeaderSize;

	// Move PMb forward
	PMb = PMb->NextBlock;

	// Keep track of used block header
	PMb->PreviousUsedBlock = TempAllocatedBlock;

	// Set properties of the new block
	PMb->NextBlock = NULL;
	PMb->Size = TempSize;

	// Is this the first block?
	if(PMb->PreviousBlock == NULL)
		StartBlock = PMb;
	else
		// Point back to the start
		PMb = StartBlock;

	// Save
	MemBlock = PMb;

	return ReturnAddress;

	/*
	// No free block was found so a new one is created
	PMb = ALIGN_MEMORY((ULONG)PMb + SizeToBeAllocated + HeaderSize);
	PMb->IsFree = 0;
	*/

	/*
	PMb->NextBlock = MemBlock;
	MemBlock = PMb;

	return PMb;
	*/
	
	// Old code
	/*
    PVOID returnPointer = freeMemPointer;                   // Give current address to object
	freeMemPointer = ALIGN_MEMORY((ULONG)freeMemPointer + size); // Increase freeMemPointer	
	return returnPointer;
	*/
}


// I'm on it...
VOID
MmFree(PVOID BlockBody)
{
	PMEMORY_BLOCK PMb = MemBlock;
	PVOID BlockHeader = (PCHAR)BlockBody - HeaderSize;
	PMb = BlockHeader;

	// Add the block to the free ones
	PMb->NextBlock->PreviousBlock = PMb;

	// Append neighbor block?
	if(PMb + PMb->Size + HeaderSize == PMb->NextBlock) {
		PMb->Size = PMb->NextBlock->Size + HeaderSize;
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