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

	const uint size_with_main_allocator = size + sizeof(allocators::LinearAllocator);
	mpMemoryStart = malloc(size_with_main_allocator);

	// Sanity check creation
	if (!mpMemoryStart)
	{
		return false;
	}

	mMemorySize = size_with_main_allocator;

	

	// Sanity check heap initialization
	if (!mMainHeap.Initialize(mpMemoryStart, mMemorySize))
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
		mInstance->EndInternal();
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
	return mMainHeap.Allocate(size);
}

void memory::MemoryManager::Free(void* ptr)
{
	IF_NOT_INITIALIZED_RETURN_VOID;
	mMainHeap.Free(ptr);
}

memory::Heap* memory::MemoryManager::CreateHeap(uint size)
{
	IF_NOT_INITIALIZED_RETURN(nullptr);
	return mMainHeap.CreateHeap(size);
}



/*
	Getters
*/

uint memory::MemoryManager::GetMainHeapSize()
{
	// TODO
	return 0;
}

uint memory::MemoryManager::GetMainAllocatorSize()
{
	// TODO
	return 0;
}

uint memory::MemoryManager::GetTotalAllocatedMemorySize()
{
	// TODO
	return 0;
}



/*
	Private constructor and destructor
*/

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
	free(mpMemoryStart);
	mMemorySize = 0;
	mpMemoryStart = nullptr;
}