#include "LinearAllocator.h"

using namespace memory;

allocators::LinearAllocator::LinearAllocator() : 
	mCurrent(nullptr)
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

bool allocators::LinearAllocator::Initialize(void* memoryStart, uint memorySize, bool memoryIncludesAllocator)
{
	/*
		If the allocator ifself is included in its manageable memory, offset memory start by the size
		of the allocator.
	*/

	if (memoryIncludesAllocator)
	{
		/*
			TODO: Calculate and add offset for free memory data structure.
		*/
		mpMemoryStart = mCurrent = (char*)memoryStart + sizeof(LinearAllocator);
	}
	else
	{
		mpMemoryStart = mCurrent = memoryStart;
	}

	mMemorySize = memorySize;

	/*
		TODO: Set up data structure for free memory
	*/

	return true;
}

void* memory::allocators::LinearAllocator::Allocate(uint size)
{
	// Sanity check if allocation fits in free memory
	if (mMemoryUsed + size > mMemorySize)
	{
		return nullptr;
	}

	void* p = mCurrent;
	mCurrent = (char*)mCurrent + size;

	/*
		TODO: Update free memory data structure
	*/

	return p;
}

void memory::allocators::LinearAllocator::Free(void* ptr)
{
	/*
		TODO: Santiy check if ptr belongs to this memory.
		TODO: Update free memory data structure.
	*/
}
