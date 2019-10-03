#include "Heap.h"

void* memory::heaps::Heap::Allocate(uint size)
{
	return mpAllocator->Allocate(size);
}

void memory::heaps::Heap::Free(void* ptr)
{
	mpAllocator->Free(ptr);
}

memory::heaps::Heap* memory::heaps::Heap::CreateHeap(uint size)
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

	heaps::Heap* p_heap = new(p_reserved_mem) heaps::Heap();

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

