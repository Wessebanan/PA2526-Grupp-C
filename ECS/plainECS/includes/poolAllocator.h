#pragma once

#include <memory>

class PoolAllocator
{
public:
	PoolAllocator();
	~PoolAllocator();

	void* allocate();
	void free(void* _ptr);

	void reset();
	void initialize(void* _memoryStart, size_t _memorySize, size_t _objectSize);

	size_t getUsedMemory();
	size_t getAllocations();

private:
	void* memoryStart;

	size_t memoryAllocations;
	size_t memorySize;
	size_t memoryUsed;
	size_t objectSize;
	size_t objectCap;

	void** freeList;

	inline void* indexToPtr(size_t _index)
	{
		return (void*)((char*)memoryStart + objectSize * _index);
	}

	inline void* pointerIncrement(void* _ptr, size_t _size)
	{
		return (void*)((char*)_ptr + _size);
	}
};