/*
        Module: mm.c
        Author: Hugo Heyman


		// I know I need to comment a lot of things that I've changed lately... will be done in a few mins
		
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
	//MemBlock->StartBlock = MemBlock;
	MemBlock->Size = MAIN_MEM_SIZE - sizeof(MEMORY_BLOCK);
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
	
	// Search for a free block to use
	while (PMb->NextBlock != NULL) {

		// Block big enough and free?
		if(PMb->Size > SizeToBeAllocated + HeaderSize) {
			ULONG FragmentSize = PMb->Size - (SizeToBeAllocated + HeaderSize);

			// Is the fragment block bigger than the header?
			if (FragmentSize > HeaderSize) {

				// Alloc the fragment block
				ReturnAddress = PMb + HeaderSize;
				PMb = ALIGN_MEMORY((ULONG)PMb + SizeToBeAllocated + HeaderSize);
				PMb->Size = FragmentSize;

				// Point back to the start and "save"
				PMb = StartBlock;
				MemBlock = PMb;

				return ReturnAddress;

				/*
				// Create and alloc the fragment block
				PMEMORY_BLOCK FragmentMb;
				FragmentMb= ALIGN_MEMORY((ULONG)PMb + FragmentSize);
				FragmentMb->NextBlock = PMb->NextBlock;
				FragmentMb->PreviousBlock = PMb;
				FragmentMb->IsFree = 1;
				*/
				
				/*
				// Put the fragment block in the list and make it free
				PMb->NextBlock = FragmentMb;
				PMb->IsFree = 0;
				return PMb;
				*/
			}

			// ..otherwise alloc the full block (make no fragment block)
			else {
				ReturnAddress = PMb + HeaderSize;

				// Remove block from list
				PMb->NextBlock->PreviousBlock = PMb->PreviousBlock;
				PMb->PreviousBlock->NextBlock = PMb->NextBlock;

				// Point back to the start and "save"
				PMb = StartBlock;
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
	PVOID TempAllocatedBlock = PMb;
	PMb->NextBlock = ALIGN_MEMORY((ULONG)PMb + SizeToBeAllocated + HeaderSize);
	ReturnAddress = PMb + HeaderSize;

	// Move PMb forward
	PMb = PMb->NextBlock;
	PMb->NextBlock = NULL;

	// Is this the first block?
	if(PMb->PreviousBlock == NULL)
		StartBlock = PMb;

	// Keep track of used block header
	PMb->PreviousBlock = TempAllocatedBlock;

	// Point back to the start and "save"
	PMb = StartBlock;
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
	PVOID BlockHeader = BlockBody - HeaderSize;
	PMb = BlockHeader;
	if(PMb->PreviousBlock == NULL) {
		
		// Append neighbor block?
		if(PMb + PMb->Size + HeaderSize == PMb->NextBlock) {
			PMb->Size = PMb->NextBlock->Size + HeaderSize;
			PMb->NextBlock = PMb->NextBlock->NextBlock;
		}

		StartBlock = PMb;
		MemBlock = PMb;
	}
	else
	{
		// Append neighbor block?
		if(PMb + PMb->Size + HeaderSize == PMb->NextBlock) {
			PMb->Size = PMb->NextBlock->Size + HeaderSize;
			PMb->NextBlock = PMb->NextBlock->NextBlock;
		}

		PMb->PreviousBlock->NextBlock = PMb;
		MemBlock = StartBlock;
	}
	
}
