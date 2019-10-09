#include "Heap.h"

void* memory::Heap::Allocate(uint size)
{
	return mpAllocator->Allocate(size);
}

memory::Heap* memory::Heap::CreateHeap(uint size)
{
	//// TEMPORARY BACKEND START ////
	const uint heap_object_size = sizeof(Heap);
	const uint allocator_size = sizeof(allocators::LinearAllocator);

	// Allocate memory for new heap, plus the header for it
	void* p_reserved_mem = mpAllocator->Allocate(size + heap_object_size + allocator_size);

	// Sanity check and handle faulty allocation
	if (!p_reserved_mem)
	{
		return nullptr;
	}

	Heap* p_heap = new(p_reserved_mem) Heap();

	/*
		The heap itself does not know where it is placed. The heap only want to
		know where its memory block (HEADER BLOCK + ALLOCATION BLOCK) to manage is
	*/
	void* p_memory_block = (void*)((char*)p_reserved_mem + heap_object_size);
	
	// Sanity check and handle faulty initialization
	if (!p_heap->Initialize(p_memory_block, size + allocator_size))
	{
		mpAllocator->Free(p_reserved_mem);
		return nullptr;
	}
	///// TEMPORARY BACKEND END /////


	return p_heap;
}

memory::Heap::Heap() :
	mpAllocationBlockStart(nullptr),
	mpMemoryStart(nullptr),
	mpAllocator(nullptr)
{
	//
}

memory::Heap::~Heap()
{
	//// TEMPORARY BACKEND START ////

	if (mpAllocator)
	{
		mpAllocator->Terminate();
		mpAllocator->~Allocator();
		mpAllocator = nullptr;
	}
		

	///// TEMPORARY BACKEND END /////
}

bool memory::Heap::Initialize(void* memoryStart, uint memorySize)
{
	//// TEMPORARY BACKEND START ////
	mpMemoryStart = memoryStart;
	mpAllocationBlockStart = (void*)((char*)mpMemoryStart + sizeof(allocators::LinearAllocator));

	mpAllocator = new(mpMemoryStart) allocators::LinearAllocator();
	mpAllocator->Initialize(mpAllocationBlockStart, memorySize);

	///// TEMPORARY BACKEND END /////

	return true;
}

void memory::Heap::Terminate()
{
	if (mpAllocator)
	{
		mpAllocator->Terminate();
		mpAllocator->~Allocator();
		mpAllocator = nullptr;
	}
}