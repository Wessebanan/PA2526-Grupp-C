#include "FreeListAllocator.h"

memory::allocators::FreeListAllocator::FreeListAllocator() :
	mpFreeBlocks(nullptr), Allocator()
{
	//
}

memory::allocators::FreeListAllocator::~FreeListAllocator()
{
	//
}

/*
	-- Initialize
	An allocator can be used when it's given some memory to work with,
	and the size of that memory.
	This methods set all necessary data in order to work with the memory, 
	and calls Clear() in order to initialize the free list.
*/
bool memory::allocators::FreeListAllocator::Initialize(void* memoryStart, uint memorySize)
{
	mpMemoryStart = memoryStart;
	mMemorySize = memorySize;
	Clear();

	return true;
}

/*
	-- Terminate
	Clears all meta data in the allocator, making it unable to allocate until
	another Initialize() is called.
*/
void memory::allocators::FreeListAllocator::Terminate()
{
	mpMemoryStart = mpFreeBlocks = nullptr;
	mMemorySize = mMemoryUsed = 0;
	Allocator::Terminate();
}


/*
	-- Clear
	Forces all allocations to be forgotten in the memory, making all of the memory
	available for allocations.
*/
void memory::allocators::FreeListAllocator::Clear()
{
	mMemoryUsed = 0;
	mpFreeBlocks = new(mpMemoryStart) FreeBlock(mMemorySize);
}

/*
	-- Allocate
	Reserves a chunk of memory with a requested size and returns a pointer to it.
	In order to free the memory later, we allocate some more memory in order to
	store an allocation header before the actual allocation that is send back that
	will store information about the allocation.

	In memory, an allocation for an int looks like:
	|//////| Header(size=4) | int(1337) |/////////////|		(/// = Other allocations)
	                        ^
	                Returning pointer
*/
void* memory::allocators::FreeListAllocator::Allocate(uint size)
{
	// Calculate the needed allocation size, including the header.
	const uint allocation_size = size + sizeof(AllocationHeader);

	// Sanity check if allocation is possible
	if (mMemoryUsed + allocation_size > mMemorySize || !mpFreeBlocks)
	{
		return nullptr;
	}

	// Create a pointer that will store the returning address
	void* p_allocation = nullptr;

	FreeBlock* p_block = mpFreeBlocks;

	/*
		First, we check if the first block fits the required size.
		If not, start iterating the list.

		Notation:
		If the size of the block equals the requested allocation size,
		then we just remove the free block from the free list and link
		the previous block (or mpFreeBlocks) with the next block.

		If the size of the block is larger than the requested allocation
		size, then we just push the start of the free block in memory with
		the requested size.
	*/
	if (p_block->size == allocation_size)
	{
		/*
			Size of first block is equal to the allocation size.
			Pop the first free block from free list.
		*/
		mpFreeBlocks = p_block->pNext;
		p_allocation = MakeAllocationAt(p_block, size);
	}
	else if (p_block->size > allocation_size)
	{
		/*
			Size of first block is larger than the allocation size
			Offset the start of the first free block by the size of an
			allocation.
		*/

		// Push start of free block in memory by size
		void* p_new_free_block_position = (void*)((char*)p_block + allocation_size);
		mpFreeBlocks = new(p_new_free_block_position) FreeBlock(p_block->size - allocation_size, p_block->pNext);

		p_allocation = MakeAllocationAt(p_block, size);
	}
	else
	{
		/*
			First block does not fit the requested size
			Start iterating the free list to find the first suitable free block.
		*/

		/*
			Store the last block, as we need to update its pNext when the change
			the block after it.
		*/
		FreeBlock* p_last_block = p_block;
		p_block = p_block->pNext;

		/*
			End conditions for loop:
				1. p_allocation != nullptr
				2. Stepping past the last free block, e.g. no free block fits the
				   requested allocation -> return nullptr.
		*/
		do
		{
			/*
				If p_block is nullptr, we have reached the last block and found out
				that there are no free blocks that will fit the requested allocation
				size.
			*/
			if (!p_block)
			{
				return nullptr;
			}

			/*
				Same as before, except we update p_last_block instead of mpFreeBlocks.
			*/
			if (p_block->size == allocation_size)
			{
				/*
					Pop current block from free list.
					User pointer to current block to place a header.
				*/
				p_last_block->pNext = p_block->pNext;
				p_allocation = MakeAllocationAt(p_block, size);
			}
			else if (p_block->size > allocation_size)
			{
				/*
					Place allocation header at position of p_block.
					Create a new FreeBlock right after the allocation.
					Link together last free block with new free block.
				*/
				p_allocation = MakeAllocationAt(p_block, size);

				// Push start of free block in memory by size
				void* p_new_free_block_position = (void*)((char*)p_block + allocation_size);
				p_last_block->pNext = new(p_new_free_block_position) FreeBlock(p_block->size - allocation_size, p_block->pNext);

			}
		} while (!p_allocation);
	}

	return p_allocation;
}

/*
	-- Free
	Releases the reservation of an allocation, making it free for future allocations.
	Start with checking if the allocation pointer belogn
*/
void memory::allocators::FreeListAllocator::Free(void* pObject)
{
	/*
		Sanity check input, don't handle nullptr or any pointer that
		does not belong to this allocator's memory.
	*/
	if (!IsAllocatedByMe(pObject))
	{
		return;
	}

	AllocationHeader* p_header = FetchHeader(pObject);

	const uint allocation_size = p_header->size + sizeof(AllocationHeader);

	mMemoryUsed -= allocation_size;
	
	FreeBlock* p_free_block = new(p_header) FreeBlock(allocation_size);

	if (mpFreeBlocks)
	{
		p_free_block->pNext = mpFreeBlocks;
		mpFreeBlocks = p_free_block;
	}
	else
	{
		mpFreeBlocks = p_free_block;
	}
}
