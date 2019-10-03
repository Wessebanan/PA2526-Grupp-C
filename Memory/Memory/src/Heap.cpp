#include "Heap.h"

void* memory::heaps::Heap::Allocate(uint size)
{
	return mpAllocator->Allocate(size);
}

void memory::heaps::Heap::Free(void* ptr)
{
	mpAllocator->Free(ptr);
}

memory::heaps::Heap::Heap() :
	mpAllocationBlockStart(nullptr),
	mpMemoryStart(nullptr),
	mpAllocator(nullptr)
{
	//
}

memory::heaps::Heap::~Heap()
{
	//// TEMPORARY BACKEND START ////

	mpAllocator->Terminate();

	///// TEMPORARY BACKEND END /////
}

bool memory::heaps::Heap::Initialize(void* memoryStart, uint memorySize)
{
	//// TEMPORARY BACKEND START ////
	
	mpMemoryStart = memoryStart;
	mpAllocationBlockStart = (void*)((char*)mpMemoryStart + sizeof(allocators::LinearAllocator));

	mpAllocator = new(mpMemoryStart) allocators::LinearAllocator();
	mpAllocator->Initialize(mpAllocationBlockStart, memorySize);

	///// TEMPORARY BACKEND END /////

	return true;
}

