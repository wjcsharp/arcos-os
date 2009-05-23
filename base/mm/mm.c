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

// Pointer to the first block
PVOID StartBlock;

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
	
	// Keeps track of the first block
	StartBlock = MemBlock;
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
		KdPrint("inte NULL!");
		// Get the "real" block
		PMb = ALIGN_MEMORY(PVMb - HeaderSize);
		
		KdPrint("PMb: 0x%x", PMb);

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

	// Size of header for the virtual blocks
	static ULONG VirtualHeaderSize = sizeof(VIRTUAL_MEMORY_BLOCK);

	PVIRTUAL_MEMORY_BLOCK PreviousPVMb;

	// Get a copy of the block list
	PVIRTUAL_MEMORY_BLOCK PVMb = BlockOwner->AllocatedMemory;
	
	// Make a pointer to the first block
	PVOID VirtualStartBlock = PVMb;
	
	KdPrint("PVMb: 0x%x \tBlockBody: 0x%x", PVMb, BlockBody); 
	//PreviousPVMb = PVMb;

	// Find the block to be freed in the list
	while(ALIGN_MEMORY((ULONG)BlockBody - VirtualHeaderSize) != PVMb) {
		KdPrint("P: 0x%x", PVMb);
		PreviousPVMb = PVMb;
		PVMb = PVMb->NextBlock;
		KdPrint("banan: 0x%x", ALIGN_MEMORY((ULONG)BlockBody - VirtualHeaderSize));
	}
	KdPrint("babababab!");
	// Free it and remove it from the list
	//PVMb = PVMb->NextBlock;
	KdPrint("huh?");
	if(PVMb->NextBlock != NULL) 
		PreviousPVMb->NextBlock = PVMb->NextBlock;
	KdPrint("huh?");
	MmFree(PVMb);
	
	// Point to the first block
	PVMb = VirtualStartBlock;
	BlockOwner->AllocatedMemory = PVMb;
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
			// Is the fragment block bigger than the header?
			if (FragmentSize > HeaderSize) {

				// Return block body
				ReturnAddress = ALIGN_MEMORY((ULONG)PMb + HeaderSize);
				RtlFillMemory(ReturnAddress, 'C', SizeToBeAllocated);

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

				// Save block list and return block body
				MemBlock = PMb;
				return ReturnAddress;
			}

			// Otherwise alloc the full block (make no fragment block)
			else {
				// Return block body
				ReturnAddress = ALIGN_MEMORY(PMb + HeaderSize);

				// The only block property to be set
				PMb->IsFree = FALSE;

				// Is this the first block?
				if(PMb->PreviousBlock == NULL)
					StartBlock = PMb;
				else
					// Point back to the start
					PMb = StartBlock;

				// Save block list and return block body
				MemBlock = PMb;
				return ReturnAddress;
			}
		}

		// Check the next block
		else {
			PMb = PMb->NextBlock;
		}
	}
	
	// All memory used?
	if(PMb->Size > (ULONG)ALIGN_MEMORY(SizeToBeAllocated + HeaderSize)) {

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

		// Save block list and return block body
		KdPrint("xPMb: 0x%x", PMb);
		MemBlock = PMb;
		return ReturnAddress;
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
	if(PMb->NextBlock->IsFree == TRUE) {
		//KdPrint("...appending with my next neighbor");
		PMb->Size = (ULONG)ALIGN_MEMORY(PMb->Size + PMb->NextBlock->Size + HeaderSize); 
		PMb->NextBlock = PMb->NextBlock->NextBlock;
		if(PMb->NextBlock->NextBlock != NULL)
			PMb->NextBlock->NextBlock->PreviousBlock = PMb;
	}
	
	// Append previous neighbor block?
	if(PMb->PreviousBlock != NULL) { 
		if(PMb->PreviousBlock->IsFree == TRUE) {
			//KdPrint("...appending with my previous neighbor");
			PMb = PMb->PreviousBlock;
			PMb->Size = (ULONG)ALIGN_MEMORY(PMb->Size + PMb->NextBlock->Size + HeaderSize);
			PMb->NextBlock = PMb->NextBlock->NextBlock;
			if(PMb->NextBlock->NextBlock != NULL)
				PMb->NextBlock->NextBlock->PreviousBlock = PMb;
		}
	}
	
	//KdPrint("Now I'm free! (0x%x)", ALIGN_MEMORY((ULONG)PMb + HeaderSize));

	// Save the block list
	PMb = StartBlock;
	MemBlock = PMb;
}