#include "MemoryAPI.h"

#include <iostream>
using namespace std;

//Initialite instance at start
memory::MemoryManager* memory::MemoryManager::mInstance = nullptr;

memory::MemoryManager& memory::MemoryManager::instance()
{
	if (!mInstance)
	{
		#ifdef _DEBUG
			DEBUG_CONSOLE_LINE("Creating MemoryManager instance.")
		#endif
		mInstance = new MemoryManager();
	}
	return *mInstance;
}

void memory::MemoryManager::end()
{
	if (mInstance)
	{
		delete mInstance;
	}
}

memory::MemoryManager::MemoryManager() :
	mHeapSize(sumAllDomainSizes())
{
	#ifdef _DEBUG
		DEBUG_CONSOLE_LINE("Allocating main heap of size " << mHeapSize << " bytes (" << ((float)mHeapSize / 1000000) << " MB).")
	#endif

	mpHeapStart = malloc(mHeapSize);

	if (!mHeap.initialize(mpHeapStart, mHeapSize))
	{
		// TODO: Handle or throw

		#ifdef _DEBUG
				DEBUG_CONSOLE_LINE("Error, could not initialize main heap!")
		#endif

		return;
	}


	#ifdef _DEBUG
		DEBUG_CONSOLE_LINE("Begin domain allocations:")
		DEBUG_CONSOLE_LINE("\tDomain\t|\tSize\t|\tStatus")
		DEBUG_CONSOLE_LINE("\t-----------------------------------------")
	#endif

	/*
		Iterate through all defined custom heaps and allocate memory
		for them in main heap.
	*/
	using DomainPair = std::pair<MemoryDomain, DomainInfo>;
	for (DomainPair domain : MEMORY_DOMAIN_SIZES)
	{
		#ifdef _DEBUG
			DEBUG_CONSOLE("\t" << domain.second.domainString << "\t\t" << domain.second.size << "\t\t")
		#endif

		void* heap_memory = mHeap.allocate(domain.second.size);

		if (!heap_memory)
		{
			// Could not allocate, handle error
			#ifdef _DEBUG
				DEBUG_CONSOLE_LINE("FAILED")
			#endif
			continue;
		}

		heaps::Heap* new_heap = nullptr;
		switch (domain.second.type)
		{
		case HeapTypes::Linear:
			new_heap = new heaps::LinearHeap();
			break;

		default:
			#ifdef _DEBUG
				DEBUG_CONSOLE_LINE("FAILED")
			#endif
			continue;
		}

		new_heap->initialize(heap_memory, domain.second.size);
		new_heap->heapString = domain.second.domainString;
		mDomains[domain.first] = new_heap;

		#ifdef _DEBUG
			DEBUG_CONSOLE_LINE("OK")
		#endif
	}
}

memory::MemoryManager::~MemoryManager()
{
	cout << "Freeing main heap." << endl;
	free(mpHeapStart);

	for (std::pair<MemoryDomain, heaps::Heap*> d : mDomains)
	{
		cout << "Freeing domain heap " << d.second->heapString << endl;
		delete d.second;
	}
}


