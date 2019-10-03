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
	//// TEMPORARY BACKEND START ////
	Clear();
	///// TEMPORARY BACKEND END /////

	mpCurrent = nullptr;
	Allocator::Terminate();
}

void memory::allocators::LinearAllocator::Clear()
{
	/*
		TODO: Reset data structure for free memory
	*/
	
	//// TEMPORARY BACKEND START ////
	for (void* p : mAllocations)
	{
		free(p);
	}
	mAllocations.clear();
	///// TEMPORARY BACKEND END /////

	mMemoryUsed = 0;
}

void* memory::allocators::LinearAllocator::Allocate(uint size)
{
	// Sanity check if allocation fits in free memory
	if (mMemoryUsed + size > mMemoryHeapSize)
	{
		return nullptr;
	}

	/*
		TODO: Update free memory data structure
	*/


	//// TEMPORARY BACKEND START ////

	void* p = malloc(size);
	mMemoryUsed += size;

	// Store pointer in order to avoid memory leaks, free them in Free() and destructor
	mAllocations.push_back(p);

	///// TEMPORARY BACKEND END /////

	return p;
}

void memory::allocators::LinearAllocator::Free(void* ptr)
{
	/*
		TODO: Santiy check if ptr belongs to this memory.
		TODO: Update free memory data structure.
	*/

	//// TEMPORARY BACKEND START ////
	/*
		Iterate through all existing allocations and find the
		given ptr. If found, free its memory and remove it
		from the allocation list.

		If not found, nothing has to be done with the ptr
		as it's not this allocator's responsibility.
		(since the ptr is not allocated from this allocator)
	*/
	for (size_t i = 0; i < mAllocations.size(); i++)
	{
		if (mAllocations[i] == ptr)
		{
			free(ptr);
			mAllocations.erase(mAllocations.begin() + i);
		}
	}
	///// TEMPORARY BACKEND END /////
}
