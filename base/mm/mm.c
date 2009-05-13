/*
        Module: mm.c
        Author: Hugo Heyman


		Please comment on the MmAlloc.
		I know the block list is only single linked so far, I'll add PreviousBlock refering when making the Free
		
*/

#include <arcos.h> 
#include <mm.h>
#include <hal.h>        // For HalGetFirstUsableMemoryAdress().


// First free memory address
PVOID FirstMemPointer; 

// Memory Block list
PMEMORY_BLOCK MemBlock;

//
// aligns memory pointer at a 4-byte boundary
//
#define ALIGN_MEMORY(X)     ((PVOID)(((ULONG)(X) + 3) & ~3))

VOID
MmInitialize()
{
	FirstMemPointer = HalGetFirstUsableMemoryAddress();
	MemBlock = ALIGN_MEMORY((ULONG)MemBlock + FirstMemPointer + MAIN_MEM_SIZE);
	MemBlock->NextBlock = NULL;
	MemBlock->PreviousBlock = NULL;
	MemBlock->IsFree = 1; 
}


/*
PULONG
MmGetMemPointer(){
        return freeMemPointer;
}
*/

PVOID MmAlloc(ULONG SizeToBeAllocated) {
	
	// Size of the header
	ULONG HeaderSize = sizeof(PMEMORY_BLOCK);
	
	// Copy of the mb-list
	PMEMORY_BLOCK PMb = MemBlock;
	
	// Search for a free block to use
	while (PMb != FirstMemPointer) {
		// Block big enough and free?
		if(PMb->Size > SizeToBeAllocated + HeaderSize && PMb->IsFree == 1) {
			ULONG FragmentSize = PMb->Size - (SizeToBeAllocated + HeaderSize);
			// Is the fragment block bigger than the header?
			if (FragmentSize > HeaderSize) {
				// Alloc the requested block
				PMb = ALIGN_MEMORY((ULONG)PMb + SizeToBeAllocated + HeaderSize);

				// Create and alloc the fragment block
				PMEMORY_BLOCK FragmentMb;
				FragmentMb= ALIGN_MEMORY((ULONG)PMb + FragmentSize);
				FragmentMb->NextBlock = PMb->NextBlock;
				FragmentMb->IsFree = 1;

				// Put the fragment block in the list and make it free
				PMb->NextBlock = FragmentMb;
				PMb->IsFree = 0;
				return PMb;
			}
			// ..otherwise alloc the full block (make no fragment block)
			else {
				PMb = ALIGN_MEMORY((ULONG)PMb + SizeToBeAllocated + HeaderSize + FragmentSize);
				PMb->IsFree = 0;
				return PMb;
			}
		}
		// Check the next block
		else {
			PMb = PMb->NextBlock;
		}
	}
	
	// No free block was found so a new one is created
	PMb = ALIGN_MEMORY((ULONG)PMb + SizeToBeAllocated + HeaderSize);
	PMb->IsFree = 0;
	
	
	PMb->NextBlock = MemBlock;
	MemBlock = PMb;
	return PMb;
	
	// Old code
	/*
    PVOID returnPointer = freeMemPointer;                   // Give current address to object
	freeMemPointer = ALIGN_MEMORY((ULONG)freeMemPointer + size); // Increase freeMemPointer	
	return returnPointer;
	*/
}

VOID
MmFree(
	PVOID objectHeader
	)
{
	// Add code here.
}
