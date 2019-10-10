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

bool memory::allocators::FreeListAllocator::Initialize(void* memoryStart, uint memorySize)
{
	mpMemoryStart = memoryStart;
	mMemorySize = memorySize;
	Clear();

	return true;
}

void memory::allocators::FreeListAllocator::Terminate()
{
	mpMemoryStart = mpFreeBlocks = nullptr;
	mMemorySize = mMemoryUsed = 0;
	Allocator::Terminate();
}

void memory::allocators::FreeListAllocator::Clear()
{
	mMemoryUsed = 0;
	mpFreeBlocks = new(mpMemoryStart) FreeBlock(mMemorySize);
}

void* memory::allocators::FreeListAllocator::Allocate(uint size)
{
	const uint allocation_size = size + sizeof(AllocationHeader);

	// Sanity check if allocation is possible
	if (mMemoryUsed + allocation_size > mMemorySize || !mpFreeBlocks)
	{
		return nullptr;
	}

	void* p_allocation = nullptr;

	// Iterate free blocks until we find a block that fits size
	FreeBlock *p_block = nullptr, *p_last_block = nullptr;

	p_block = mpFreeBlocks;

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
		// Size of first block is equal to the allocation size
		mpFreeBlocks = p_block->pNext;
		p_allocation = MakeAllocationAt(p_block, size);
	}
	else if (p_block->size > allocation_size)
	{
		// Size of first block is larger than the allocation size

		// Push start of free block in memory by size
		void* p_new_free_block_position = (void*)((char*)p_block + allocation_size);
		mpFreeBlocks = new(p_new_free_block_position) FreeBlock(p_block->size - allocation_size, p_block->pNext);

		p_allocation = MakeAllocationAt(p_block, size);
	}
	else
	{
		// First block does not fit the requested size, start iterating the list

		p_last_block = p_block;
		p_block = p_block->pNext;

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

			if (p_block->size == allocation_size)
			{
				p_last_block->pNext = p_block->pNext;
				p_allocation = MakeAllocationAt(p_block, size);
			}
			else if (p_block->size > allocation_size)
			{		
				p_allocation = MakeAllocationAt(p_block, size);

				// Push start of free block in memory by size
				void* p_new_free_block_position = (void*)((char*)p_block + allocation_size);
				p_last_block->pNext = new(p_new_free_block_position) FreeBlock(p_block->size - allocation_size, p_block->pNext);

			}
		} while (!p_allocation);
	}

	intptr_t last_address = reinterpret_cast<intptr_t>((void*)((char*)mpMemoryStart + mMemorySize));
	intptr_t allocation_address = reinterpret_cast<intptr_t>(p_allocation);

	return p_allocation;
}

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
