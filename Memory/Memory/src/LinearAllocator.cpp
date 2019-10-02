#include "LinearAllocator.h"

using namespace memory;

allocators::LinearAllocator::LinearAllocator() : 
	mpCurrent(nullptr)
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
			TODO: Check if size of allocator is larger than memorySize, and handle it.
		*/
		mpMemoryStart = mpCurrent = (char*)memoryStart + sizeof(LinearAllocator);
	}
	else
	{
		mpMemoryStart = mpCurrent = memoryStart;
	}

	mMemorySize = memorySize;

	/*
		TODO: Set up data structure for free memory
	*/

	return true;
}

void memory::allocators::LinearAllocator::Wipe()
{
	mpCurrent = nullptr;
	Allocator::Wipe();
}

void* memory::allocators::LinearAllocator::Allocate(uint size)
{
	// Sanity check if allocation fits in free memory
	if (mMemoryUsed + size > mMemorySize)
	{
		return nullptr;
	}

	void* p = mpCurrent;
	mpCurrent = (char*)mpCurrent + size;
	mMemoryUsed += size;

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
