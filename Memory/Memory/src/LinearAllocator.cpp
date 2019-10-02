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
		or now, reset next free memory ptr at the beginning
		f the heap start.
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
