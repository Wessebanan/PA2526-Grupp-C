#include "poolAllocator.h"

PoolAllocator::PoolAllocator()
{
	freeList = nullptr;
	memoryStart = nullptr;
	memorySize = memoryUsed = objectSize = objectCap = memoryAllocations = 0;
}

PoolAllocator::~PoolAllocator()
{
	freeList = nullptr;
}

void* PoolAllocator::allocate()
{
	if (freeList == nullptr) return nullptr;

	void* newAllocation = freeList;
	freeList = (void**)(*freeList); // Increment free list (may not be in order)

	memoryUsed += objectSize;
	memoryAllocations++;
	return newAllocation;
}

void PoolAllocator::free(void* _ptr)
{
	std::memset(_ptr, 0, objectSize);
	*((void**)_ptr) = freeList;
	freeList = (void**)_ptr;
	memoryUsed -= objectSize;
	memoryAllocations--;
}

void PoolAllocator::reset()
{
	memoryUsed = 0;

	objectCap = memorySize / objectSize;

	freeList = (void**)memoryStart;

	// Make each free chunk in the memory be a pointer to the next available
	// chunk. The free list exists in non-allocated chunks within memory.
	void** p = freeList;
	for (size_t i = 0; i < objectCap - 1; i++)
	{
		// 1. Make this entry point to next chunk
		// 2. Step to next chunk
		*p = pointerIncrement(p, objectSize);
		p = (void**)*p;						  
	}
	*p = nullptr; // Last chunk is nullptr.
}

void PoolAllocator::initialize(void* _memoryStart, size_t _memorySize, size_t _objectSize)
{
	memoryStart = _memoryStart;
	memorySize = _memorySize;
	objectSize = _objectSize;
	memoryAllocations = 0;
	reset();
}

size_t PoolAllocator::getUsedMemory()
{
	return memoryUsed;
}

size_t PoolAllocator::getAllocations()
{
	return memoryAllocations;
}
