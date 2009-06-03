/*
        Module: mm.c
        Author: Hugo Heyman 2009	

		
*/

#include <arcos.h> 
#include <mm.h>
#include <hal.h>        // For HalGetFirstUsableMemoryAdress().
#include <kd.h>
#include <rtl.h>


// First free memory address
static PVOID FirstMemPointer; 


// Memory Block list
PMEMORY_BLOCK MemBlock;


// Aligns memory pointer at a 4-byte boundary
#define ALIGN_MEMORY(X)     ((PVOID)(((ULONG)(X) + 3) & ~3))



VOID MmInitialize() {

	// Initialize first memory addrress
	FirstMemPointer = ALIGN_MEMORY(HalGetFirstUsableMemoryAddress());


	//Initialize the first block
	MemBlock = FirstMemPointer;
	MemBlock->Size = MAIN_MEM_SIZE - sizeof(MEMORY_BLOCK);
	MemBlock->IsFree = TRUE;
	MemBlock->NextBlock = NULL;
	MemBlock->PreviousBlock = NULL;
	
}



ULONG MmGetUsedMemSum() {

	// Copy of the mb-list
	PMEMORY_BLOCK PMb = MemBlock;


	ULONG UsedMemSum = 0;
	

	// Go through all blocks
	while(PMb != NULL) {

		if(PMb->IsFree == FALSE)
			UsedMemSum = UsedMemSum + PMb->Size + sizeof(MEMORY_BLOCK);


		PMb = PMb->NextBlock;
	}

	return UsedMemSum;
}



VOID MmPrintBlocks() {

	// Copy of the mb-list
	PMEMORY_BLOCK PMb = MemBlock;


	// Go through all blocks
	while(PMb != NULL) {


		if(PMb->IsFree == TRUE) 
			KdPrint("F\t - Block 0x%x\t - NextBlock 0x%x\t - Size %d \n"
			, ALIGN_MEMORY((ULONG)PMb + sizeof(MEMORY_BLOCK)), ALIGN_MEMORY((ULONG)PMb->NextBlock + sizeof(MEMORY_BLOCK)), PMb->Size);	

		else
			KdPrint("U\t - Block 0x%x\t - NextBlock 0x%x\t - Size %d \n"
			, ALIGN_MEMORY((ULONG)PMb + sizeof(MEMORY_BLOCK)), ALIGN_MEMORY((ULONG)PMb->NextBlock + sizeof(MEMORY_BLOCK)), PMb->Size);
		

		PMb = PMb->NextBlock;
	}
}



ULONG MmGetUsedVirtMemSum() {

	// Initialize the list of virtual memory blocks
	PVIRTUAL_MEMORY_BLOCK PVMb = KeCurrentProcess->AllocatedMemory;
	

	// These are the "real" blocks
	PMEMORY_BLOCK PMb;
	

	ULONG MemSum = 0;
	

	// Go through the list
	while(PVMb != NULL) {
		

		// Get the "real" block
		PMb = (PCHAR)PVMb - sizeof(MEMORY_BLOCK);
	

		// Add to the sum and go to the next block
		MemSum = MemSum + PMb->Size;

		PVMb = PVMb->NextBlock;
	}


	return MemSum;
}



PVOID MmVirtualAlloc(PPROCESS BlockOwner, ULONG Size) {

	// Get a copy of the block list
	PVIRTUAL_MEMORY_BLOCK PVMb = BlockOwner->AllocatedMemory;


	// Create a new block
	PVIRTUAL_MEMORY_BLOCK NewBlock;

	NewBlock = MmAlloc(Size + sizeof(VIRTUAL_MEMORY_BLOCK));


	// Add it to the list
	NewBlock->NextBlock = PVMb;
	KeCurrentProcess->AllocatedMemory = NewBlock;


	return (PCHAR)BlockOwner->AllocatedMemory + sizeof(VIRTUAL_MEMORY_BLOCK);
}



VOID MmVirtualFree(PPROCESS BlockOwner, PVOID BlockBody) {
	
	// Get a copy of the virtual blocks list
	PVIRTUAL_MEMORY_BLOCK PVMb = BlockOwner->AllocatedMemory;


	if(PVMb->NextBlock == NULL) { // First Block
		PVMb = NULL;
	}

	else if ((PCHAR)PVMb == (PCHAR)BlockBody - sizeof(VIRTUAL_MEMORY_BLOCK)) {		// Last block?
		
		PVMb = PVMb->NextBlock;
	}

	else {														// ..otherwise find the block

		// Go through blocks
		while((PCHAR)PVMb->NextBlock != (PCHAR)BlockBody - sizeof(VIRTUAL_MEMORY_BLOCK)) {

			PVMb = PVMb->NextBlock;
		}
		

		// Remove the block from virtual block list
		PVMb->NextBlock = PVMb->NextBlock->NextBlock;
	}
	

	BlockOwner->AllocatedMemory = PVMb;


	// Free the "real" block
	MmFree((PCHAR)BlockBody - sizeof(VIRTUAL_MEMORY_BLOCK));
}



VOID MmVirtualFreeAll(PPROCESS BlockOwner) {
	
	// Get the block list
	PVIRTUAL_MEMORY_BLOCK PVMb = (PVOID)BlockOwner->AllocatedMemory;
	

	// Go through the blocks remove and free them
	while(PVMb != NULL) {
		PVMb = PVMb->NextBlock;
		MmFree(PVMb);
	}


	BlockOwner->AllocatedMemory = PVMb;
}



PVOID MmAlloc(ULONG SizeToBeAllocated) {

	// Address to return
	PVOID ReturnAddress;
	

	// Copy of the memory block pointer
	PMEMORY_BLOCK PMb = MemBlock;
	

	// Temporary pointers to save block header
	PVOID TempNextBlock;


	// Allign the size of the desired allocation
	SizeToBeAllocated = (ULONG)ALIGN_MEMORY(SizeToBeAllocated);
	

	// Search for a free block to use
	while (PMb != NULL) {
		

		// Block big enough and free?
		if(PMb->IsFree) {


			if(PMb->Size >= (ULONG)ALIGN_MEMORY(SizeToBeAllocated)) {
			

				ULONG FragmentSize = PMb->Size - SizeToBeAllocated;
				

				// Is the fragment block bigger than the header?
				if (FragmentSize > sizeof(MEMORY_BLOCK)) {


					// Return block body
					ReturnAddress = (PCHAR)PMb + sizeof(MEMORY_BLOCK);


					//// Set block 1 properties ////
					PMb->IsFree = FALSE;
					PMb->Size = SizeToBeAllocated;


					// Keep track of the old "NextBlock" (block 3)
					TempNextBlock = PMb->NextBlock;


					// 1->2
					PMb->NextBlock = (PCHAR)PMb + sizeof(MEMORY_BLOCK) + SizeToBeAllocated;
					

					// 1<-2
					PMb->NextBlock->PreviousBlock = PMb;
					
					// Set Block 2 properties
					PMb->NextBlock->IsFree = TRUE;
					PMb->NextBlock->Size = FragmentSize - sizeof(MEMORY_BLOCK);
					

					// 2->3
					PMb->NextBlock->NextBlock = (PCHAR)TempNextBlock;


					if(PMb->NextBlock->NextBlock != NULL) {

						// 2<-3
						PMb->NextBlock->NextBlock->PreviousBlock = PMb->NextBlock;
					}
					
					
					return ReturnAddress;
				}

				// Otherwise alloc the full block (make no fragment block)
				else {

					// Return block body
					ReturnAddress = (PCHAR)PMb + sizeof(MEMORY_BLOCK);
					
					// The only block property to be set
					PMb->IsFree = FALSE;

					return ReturnAddress;
				}
			}
		}

		// Check the next block
		PMb = PMb->NextBlock;

	}

	return NULL;
}



VOID MmFree(PVOID BlockBody) {


	// Find block header
	PMEMORY_BLOCK PMb = (PCHAR)BlockBody - sizeof(MEMORY_BLOCK);


	// That's what I do!
	PMb->IsFree = TRUE;
	

	// Merge with next neighbor block?
	if(PMb->NextBlock != NULL) {
		if(PMb->NextBlock->IsFree == TRUE) {


			// Merge sizes
			PMb->Size = PMb->Size + PMb->NextBlock->Size + sizeof(MEMORY_BLOCK); 


			// Blocks 1-2-3

			// 1->3
			PMb->NextBlock = PMb->NextBlock->NextBlock;
			if(PMb->NextBlock != NULL)

				// 3<-1
				PMb->NextBlock->PreviousBlock = PMb;
		}
	}
	

	// Merge with previous neighbor block?
	if(PMb->PreviousBlock != NULL) { 
		if(PMb->PreviousBlock->IsFree == TRUE) {

			// Move back one block
			PMb = PMb->PreviousBlock;
			

			// Merge sizes
			PMb->Size = PMb->Size + PMb->NextBlock->Size + sizeof(MEMORY_BLOCK);


			// Blocks 1-2-3

			// 1->3
			PMb->NextBlock = PMb->NextBlock->NextBlock;
			if(PMb->NextBlock != NULL)

				// 1<-3
				PMb->NextBlock->PreviousBlock = PMb;
		}
	}
}

