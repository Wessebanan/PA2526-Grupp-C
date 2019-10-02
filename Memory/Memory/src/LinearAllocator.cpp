#include "LinearAllocator.h"

using namespace memory;

allocators::LinearAllocator::LinearAllocator() :
	mpCurrent(nullptr),
	mFreeMemBlocks_temp(nullptr),
	mUsedMemBlocks_temp(nullptr)
{
	/*
		Set all member variables to default. Initialize() will
		later set all member variables appropriately.
	*/
}

allocators::LinearAllocator::~LinearAllocator()
{
	/*
		Since all work memory for this allocator is pre-allocated, there is no need to free the
		memory. The allocator 'above' this one will handle free memory management.
	*/
}

bool allocators::LinearAllocator::Initialize(void* memoryStart, uint memorySize)
{
	/*
		TODO: Calculate and add offset for free memory data structure.
		TODO: Check if size of allocator is larger than memorySize, and handle it.
	*/

	mpMemoryBlockStart = memoryStart;
	mpMemoryHeapStart = mpCurrent = (char*)memoryStart + sizeof(LinearAllocator);
	mMemoryBlockSize = memorySize;
	mMemoryHeapSize = mMemoryBlockSize - sizeof(LinearAllocator);

	/*
		TODO: Set up data structure for free memory
	*/

	/*
		Notation, this is an temporary implementation. This is going to
		be refactored when the backend is under implementation.
	*/

	mFreeMemBlocks_temp = new MemBlock(mpMemoryHeapStart, mMemoryHeapSize);

	return true;
}

void memory::allocators::LinearAllocator::Terminate()
{
	mpCurrent = nullptr;
	Allocator::Terminate();
}

void memory::allocators::LinearAllocator::Clear()
{
	/*
		TODO: Reset data structure for free memory
	*/

	/*
		For now, reset next free memory ptr at the beginning
		of the heap start.
	*/
	mpCurrent = mpMemoryHeapStart;
	mMemoryUsed = 0;
}

void* memory::allocators::LinearAllocator::Allocate(uint size)
{
	// Sanity check if allocation fits in free memory
	if (mMemoryUsed + size > mMemoryHeapSize)
	{
		return nullptr;
	}

	//void* p = mpCurrent;
	//mpCurrent = (char*)mpCurrent + size;
	//mMemoryUsed += size;
	//return p;

	///*
	//	TODO: Update free memory data structure
	//*/

	/*
		Notation, this is an temporary implementation. This is going to
		be refactored when the backend is under implementation.
	*/

	/*
		Sanity check if there is any free blocks. If not, the allocator
		is either not initialized or the heap is completely full.
	*/
	if (!mFreeMemBlocks_temp)
	{
		return nullptr;
	}

	MemBlock* free_block = nullptr;
	MemBlock* walker = mFreeMemBlocks_temp;
	while (!free_block)
	{
		// Check if current free block is to small
		if (walker->size < size)
		{
			/*
				Block is too small. If next block is null, there are no
				available blocks that will fit the wanted allocation.
				(end of free list)
			*/
			if (!walker->next)
			{
				return nullptr;
			}
			walker = walker->next;
			continue; // Jump to next loop iteration
		}

		/*
			From here, we can safely assume the current block is will
			fit the required size.
		*/

		// Create a MemBlock for the new allocation
		free_block = new MemBlock(walker->start, size);

		// Adjust free block size
		walker->size -= size;

		/*
			Remember, we have checked that this free block size will fit the
			requested size. So, either the block occupies all the free block
			memory or only a part of it.
		*/

		/*
			If new block occupies the whole free block, remove the free block
			and link its prev and next together.
		*/
		if (walker->size == 0)
		{
			if (walker->prev && walker->next)
			{
				walker->prev->next = walker->next;
				walker->next->prev = walker->prev;
			}
			else if (walker->prev) // walker->next == nullptr	<- last
			{
				walker->prev->next = nullptr;
			}
			else if (walker->next) // walker->prev == nullptr	<- first
			{
				mFreeMemBlocks_temp = walker->next;
				walker->next->prev = nullptr;
			}
			else // walker->prev == walker->next == nullptr)	<- last and first
			{
				mFreeMemBlocks_temp = nullptr;
			}

			delete walker;
		}

		/*
			The requested size is only a part of the free block.
		*/
		else
		{
			walker->start = (void*)((char*)walker->start + size);
		}
	}

	// Push new allocated block into allocated list
	free_block->next = mUsedMemBlocks_temp;
	mUsedMemBlocks_temp->prev = free_block;
	mUsedMemBlocks_temp = free_block;

	return free_block->start;
}

void memory::allocators::LinearAllocator::Free(void* ptr)
{
	// Santiy check that ptr belongs to the allocator's memory
	uint heap_start_address = (uint)mpMemoryHeapStart;
	uint heap_end_address = (uint)((char*)mpMemoryHeapStart + mMemoryHeapSize);
	uint ptr_address = (uint)ptr;

	if (ptr_address < heap_start_address || ptr_address > heap_end_address)
	{
		return;
	}
	
	/*
		Backend is to be implemented, free memory data structure is to be
		refactored as it's just a temporary implementation.
	*/

	// Find the allocated block in order to know the size
	MemBlock* block_to_free = nullptr;
	MemBlock* walker = mUsedMemBlocks_temp;
	while (!block_to_free)
	{
		// walker == nullptr means we could not find the allocated block
		if (!walker)
		{
			return;
		}

		// If not correct, check next
		if (walker->start != ptr)
		{
			walker = walker->next;
			continue;
		}

		// Store block and remove it from allocated list
		block_to_free = walker;
		
		if (walker->prev && walker->next)
		{
			walker->prev->next = walker->next;
			walker->next->prev = walker->prev;
		}
		else if (walker->prev) // walker->next == nullptr	<- last
		{
			walker->prev->next = nullptr;
		}
		else if (walker->next) // walker->prev == nullptr	<- first
		{
			mFreeMemBlocks_temp = walker->next;
			walker->next->prev = nullptr;
		}
		else // walker->prev == walker->next == nullptr)	<- last and first
		{
			mFreeMemBlocks_temp = nullptr;
		}
	}

	// Find the correct spot in the free list for the free memory
	uint ptr_address = (uint)ptr;
	bool placed_in_list = false;
	walker = mFreeMemBlocks_temp;
	while (walker && !placed_in_list)
	{
		// TODO
	}
}
