/*
        Module: mm.c
        Author: Hugo Heyman


		Now only free blocks should be in the pointer list.
		There's a lot of commented code in MmAlloc but I don't want to remove it til I know this is somewhat working
		
*/

#include <arcos.h> 
#include <mm.h>
#include <hal.h>        // For HalGetFirstUsableMemoryAdress().


// First free memory address
static PVOID FirstMemPointer; 

// Memory Block list
PMEMORY_BLOCK MemBlock;

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
	MemBlock->StartBlock = MemBlock;
	MemBlock->Size = MAIN_MEM_SIZE - sizeof(MEMORY_BLOCK);
	MemBlock->NextBlock = NULL;
	MemBlock->PreviousBlock = NULL;
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

	// Size of the header
	ULONG HeaderSize = sizeof(MEMORY_BLOCK);
	
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
				ReturnAddress = PMb;
				PMb = ALIGN_MEMORY((ULONG)PMb + SizeToBeAllocated + HeaderSize);
				PMb->Size = FragmentSize;

				// Point back to the start and "save"
				PMb = PMb->StartBlock;
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
				ReturnAddress = PMb;

				// Remove block from list
				PMb->NextBlock->PreviousBlock = PMb->PreviousBlock;
				PMb->PreviousBlock->NextBlock = PMb->NextBlock;

				// Point back to the start and "save"
				PMb = PMb->StartBlock;
				MemBlock = PMb;

				return ReturnAddress;
			}
		}

		// Check the next block
		else {
			PMb = PMb->NextBlock;
		}
	}
	
	// All block except last one has been checked, so now alloc from last "big" block
	ReturnAddress = PMb;
	PMb = ALIGN_MEMORY((ULONG)PMb + SizeToBeAllocated + HeaderSize);

	// Point back to the start and "save"
	PMb = PMb->StartBlock;
	MemBlock = PMb;

	return ReturnAddress;

	/*
	// No free block was found so a new one is created
	PMb = ALIGN_MEMORY((ULONG)PMb + SizeToBeAllocated + HeaderSize);
	PMb->IsFree = 0;
	*/

	/*
	//*** I'm a bit unsure that these two lines does what I want ***
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
MmFree(PVOID objectHeader)
{
	// Add code here.
}
