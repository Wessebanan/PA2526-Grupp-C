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
	IF_INITIALIZED_RETURN(false);

	mpMemoryStart = malloc(size);

	// Sanity check creation
	if (!mpMemoryStart)
	{
		return false;
	}

	mMemorySize = size;

	// Sanity check heap initialization
	if (!mMemory.Initialize(mpMemoryStart, mMemorySize))
	{
		mMemorySize = 0;
		free(mpMemoryStart);
		mpMemoryStart = nullptr;
		return false;
	}

	return true;
}

void memory::MemoryManager::End()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void* memory::MemoryManager::Allocate(uint size)
{
	IF_NOT_INITIALIZED_RETURN(nullptr);

	/*
		Not need to sanity check allocation for nullptr,
		since if the allocation wasn't succeeded we want
		to return nullptr anyway.
	*/
	return mMemory.Allocate(size);
}

void memory::MemoryManager::Free(void* ptr)
{
	IF_NOT_INITIALIZED_RETURN();
	mMemory.Free(ptr);
}

memory::allocators::Allocator* memory::MemoryManager::CreateAllocator(uint size)
{
	IF_NOT_INITIALIZED_RETURN(nullptr);


	/*
		A user wants an allocator that manage a memory chunk of 'size' bytes.
		MemoryManager will put the allocator at the beginning of this memory
		chunk, hince we need to allocate for 'size' AND the size of the
		allocator.
	*/
	uint size_with_allocator = size + sizeof(allocators::LinearAllocator);

	/*
		Notation:
		This function is in beta version. For now, all heap allocatiors are linear
		allocators in disguise. In later versions of this API, there will be specific
		functions for creating different allocator types, like linear and pool.
	*/

	void* p = mMemory.Allocate(size_with_allocator);
	
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
	if (!pAllocator->Initialize(p, size_with_allocator, true))
	{
		// Return memory chunk to main heap
		mMemory.Free(p);
		return nullptr;
	}

	return pAllocator;
}

memory::MemoryManager::MemoryManager() :
	mMemorySize(0),
	mpMemoryStart(nullptr)
{
	/*
		Set all member variables to default. Initialize() will
		later set all member variables appropriately.
	*/
}

memory::MemoryManager::~MemoryManager()
{
	mMemory.Wipe();
	free(mpMemoryStart);
	mMemorySize = 0;
	mpMemoryStart = nullptr;
}