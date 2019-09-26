#include "LinearHeap.h"

using namespace memory;

heaps::LinearHeap::LinearHeap() : 
	mMemoryStart(nullptr),
	mMemorySize(0),
	mCurrent(nullptr)
{
	//
}

heaps::LinearHeap::~LinearHeap()
{
	//
}

bool heaps::LinearHeap::initialize(void* memoryStart, uint memorySize)
{
	mMemoryStart = mCurrent = memoryStart;
	mMemorySize = memorySize;

	return true;
}

void* memory::heaps::LinearHeap::allocate(uint size)
{
	if (mUsedMemory + size > mMemorySize)
	{
		return nullptr;
	}

	void* p = mCurrent;
	mCurrent = (char*)mCurrent + size;
	return p;
}
