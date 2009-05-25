/*
        Module: mm.c
        Author: Hugo Heyman


		MmVirtualAlloc and MmVirtualFree needs to be tested

		
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

VOID MmInitialize()
{
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

	// Size of the header
	static ULONG HeaderSize = sizeof(MEMORY_BLOCK);

	// Copy of the mb-list
	PMEMORY_BLOCK PMb = MemBlock;

	ULONG UsedMemSum = 0;
	
	// Go through all blocks
	while(PMb != NULL) {
		if(PMb->IsFree == FALSE)
			UsedMemSum = UsedMemSum + PMb->Size + HeaderSize;

		PMb = PMb->NextBlock;
	}

	return UsedMemSum;
}

VOID MmPrintBlocks() {
	
	// Size of the header
	static ULONG HeaderSize = sizeof(MEMORY_BLOCK);

	// Copy of the mb-list
	PMEMORY_BLOCK PMb = MemBlock;

	// Go through all blocks
	while(PMb != NULL) {
		if(PMb->IsFree == TRUE) 
			KdPrint("F\t - Block 0x%x\t - Size %d \n", ALIGN_MEMORY((ULONG)PMb + HeaderSize), PMb->Size);	
		else
			KdPrint("U\t - Block 0x%x\t - Size %d \n", ALIGN_MEMORY((ULONG)PMb + HeaderSize), PMb->Size);

		PMb = PMb->NextBlock;
	}
}

ULONG MmGetUsedVirtMemSum() {

	// Size of the header
	static ULONG HeaderSize = sizeof(MEMORY_BLOCK);

	// Initialize the list of virtual memory blocks
	PVIRTUAL_MEMORY_BLOCK PVMb = KeCurrentProcess->AllocatedMemory;
	
	PMEMORY_BLOCK PMb;
	
	ULONG MemSum = 0;
	
	// Go through the list
	while(PVMb != NULL) {
		
		//KdPrint("inte NULL!");
		
		// Get the "real" block
		//KdPrint("PVMb: 0x%x", PVMb);
		PMb = ALIGN_MEMORY((ULONG)PVMb - HeaderSize);
		
		KdPrint("PMb: 0x%x", PMb);

		KdPrint("PMb->Size: %d", PMb->Size);

		MemSum = MemSum + PMb->Size + HeaderSize;
		PVMb = PVMb->NextBlock;
	}
	return MemSum;
}


PVOID MmVirtualAlloc(PPROCESS BlockOwner, ULONG Size) {
	
	// Size of header for the virtual blocks
	static ULONG VirtualHeaderSize = sizeof(VIRTUAL_MEMORY_BLOCK);
	
	// Get a copy of the block list
	PVIRTUAL_MEMORY_BLOCK PVMb = BlockOwner->AllocatedMemory;
	
	// Create a new block
	PVIRTUAL_MEMORY_BLOCK NewBlock;
	NewBlock = MmAlloc(Size + VirtualHeaderSize);

	// Add it to the list
	NewBlock->NextBlock = PVMb;
	BlockOwner->AllocatedMemory = NewBlock;

	return ALIGN_MEMORY((ULONG)BlockOwner->AllocatedMemory + VirtualHeaderSize);
}

VOID MmVirtualFree(PPROCESS BlockOwner, PVOID BlockBody) {
	
	// Get a copy of the virtual blocks list
	PVIRTUAL_MEMORY_BLOCK PVMb = BlockOwner->AllocatedMemory;


	if ((PCHAR)PVMb == (PCHAR)BlockBody - sizeof(VIRTUAL_MEMORY_BLOCK)) {		// Is it the first block?
		PVMb = PVMb->NextBlock;
	}
	else if(PVMb->NextBlock == NULL) {							// Is there only one block?
		PVMb = NULL;
	}
	else {														// ..then it's in the middle
		
		// Go through blocks in the middle
		while((PCHAR)PVMb->NextBlock != (PCHAR)BlockBody - sizeof(VIRTUAL_MEMORY_BLOCK)) {
			PVMb = PVMb->NextBlock;
		}

		// Remove the block from virtual block list
		PVMb->NextBlock = PVMb->NextBlock->NextBlock;
	}

	// Free the "real" block
	MmFree((PCHAR)BlockBody - sizeof(VIRTUAL_MEMORY_BLOCK));
}


VOID MmVirtualFreeAll(PPROCESS BlockOwner) {
	
	// Get the block list
	PVIRTUAL_MEMORY_BLOCK PVMb = (PVOID)BlockOwner->AllocatedMemory;
	
	// Go through the blocks remove and free them
	while(PVMb != NULL) {
		MmFree(PVMb);
		PVMb = PVMb->NextBlock;
	}

}

PVOID MmAlloc(ULONG SizeToBeAllocated) {
	
	// Size of the header
	static ULONG HeaderSize = sizeof(MEMORY_BLOCK);

	// Address to return
	PVOID ReturnAddress;
	
	// Copy of the memory block pointer
	PMEMORY_BLOCK PMb = MemBlock;
	
	// Temporary pointers to save block header
	PVOID TempNextBlock;
	
	// Search for a free block to use
	while (PMb != NULL) {
		
		// Block big enough and free?
		if(PMb->IsFree) {
			if(PMb->Size >= (ULONG)ALIGN_MEMORY(SizeToBeAllocated)) {
				
				ULONG FragmentSize = (ULONG)ALIGN_MEMORY(PMb->Size - (SizeToBeAllocated + HeaderSize));

				// Is the fragment block bigger than the header?
				if (FragmentSize > HeaderSize) {

					// Return block body
					ReturnAddress = ALIGN_MEMORY((ULONG)PMb + HeaderSize);
					//RtlFillMemory(ReturnAddress, 'U', SizeToBeAllocated);

					// Set previous block properties
					PMb->IsFree = FALSE;
					PMb->Size = SizeToBeAllocated;

					// Set next block Properties
					TempNextBlock = PMb->NextBlock;
					PMb->NextBlock = ALIGN_MEMORY((ULONG)PMb + HeaderSize + SizeToBeAllocated);
					PMb->NextBlock->IsFree = TRUE;
					PMb->NextBlock->Size = FragmentSize;
					PMb->NextBlock->NextBlock = TempNextBlock;
					PMb->NextBlock->PreviousBlock = PMb;
					
					return ReturnAddress;
				}

				// Otherwise alloc the full block (make no fragment block)
				else {

					// Return block body
					ReturnAddress = ALIGN_MEMORY(PMb + HeaderSize);

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
	
	// Size of the header
	static ULONG HeaderSize = sizeof(MEMORY_BLOCK);

	// Find block header
	PVOID PHeader = ALIGN_MEMORY((PCHAR)BlockBody - HeaderSize);
	PMEMORY_BLOCK PMb = PHeader;

	// That's what I do!
	PMb->IsFree = TRUE;
	
	// Append next neighbor block?
	if(PMb->NextBlock != NULL) {
		if(PMb->NextBlock->IsFree == TRUE) {
			PMb->Size = (ULONG)ALIGN_MEMORY(PMb->Size + PMb->NextBlock->Size + HeaderSize); 
			PMb->NextBlock = PMb->NextBlock->NextBlock;
			if(PMb->NextBlock != NULL)
				PMb->NextBlock->PreviousBlock = PMb;
		}
	}
	
	// Append previous neighbor block?
	if(PMb->PreviousBlock != NULL) { 
		if(PMb->PreviousBlock->IsFree == TRUE) {
			PMb = PMb->PreviousBlock;
			PMb->Size = (ULONG)ALIGN_MEMORY(PMb->Size + PMb->NextBlock->Size + HeaderSize);
			PMb->NextBlock = PMb->NextBlock->NextBlock;
			if(PMb->NextBlock != NULL)
				PMb->NextBlock->PreviousBlock = PMb;
		}
	}
	
	//KdPrint("Now I'm free! (0x%x)", ALIGN_MEMORY((ULONG)PMb + HeaderSize));

}

