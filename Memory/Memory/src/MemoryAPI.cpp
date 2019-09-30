#include "MemoryAPI.h"

/*
	Initialite instance at start. Must be in cpp file as it will
	be set multiple times otherwise.
*/
memory::MemoryManager* memory::MemoryManager::mInstance = nullptr;

memory::MemoryManager& memory::MemoryManager::Instance()
{
	if (!mInstance)
	{
		mInstance = new MemoryManager();
	}
	return *mInstance;
}

bool memory::MemoryManager::Initialize(uint size)
{
	mpHeapStart = malloc(size);

	// Sanity check creation
	if (!mpHeapStart)
	{
		return false;
	}

	mHeapSize = size;

	// Sanity check heap initialization
	if (!mHeap.Initialize(mpHeapStart, mHeapSize))
	{
		mHeapSize = 0;
		free(mpHeapStart);
		return false;
	}

	return true;
}

void memory::MemoryManager::End()
{
	if (mInstance)
	{
		delete mInstance;
	}
}

memory::allocators::Allocator* memory::MemoryManager::CreateHeap(uint size)
{
	/*
		Notation:
		This function is in beta version. For now, all heap allocatiors are linear
		allocators in disguise. In later versions of this API, there will be specific
		functions for creating different allocator types, like linear and pool.
	*/

	void* p = mHeap.Allocate(size);
	
	// Sanity check allocation
	if (!p)
	{
		return nullptr;
	}

	/*
		Use the 'new placement operator' to allocate the new allocator. This operator
		does not allocate new memory on the system heap. Instead, it takes a pointer
		and places the new object at that memory address.
	*/

	allocators::LinearAllocator* pAllocator = new(p) allocators::LinearAllocator();

	/*
		Initialize the new allocator. By setting the last parameter to true, the allocator
		will know it itself is included in the given memory start pointer. It will counter
		this by offsetting the memory start by its own size before allocating objects.
	*/

	// Sanity check initialization
	if (!pAllocator->Initialize(p, size, true))
	{
		// Return memory chunk to main heap
		mHeap.Free(p);
		return nullptr;
	}

	return pAllocator;
}

memory::MemoryManager::MemoryManager() :
	mHeapSize(0),
	mpHeapStart(nullptr)
{
	/*
		Set all member variables to default. Initialize() will
		later set all member variables appropriately.
	*/
}

memory::MemoryManager::~MemoryManager()
{
	free(mpHeapStart);
}


