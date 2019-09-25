#include "MemoryAPI.h"

//Initialite instance at start
memory::MemoryManager* memory::MemoryManager::mInstance = nullptr;

memory::MemoryManager& memory::MemoryManager::getInstance()
{
	if (!mInstance)
	{
		mInstance = new MemoryManager();
	}
	return *mInstance;
}

memory::MemoryManager::MemoryManager() : mHeap(mpHeapStart, TOTAL_HEAP_SIZE_BYTES)
{
	
}

memory::MemoryManager::~MemoryManager()
{
	delete mInstance;
}


