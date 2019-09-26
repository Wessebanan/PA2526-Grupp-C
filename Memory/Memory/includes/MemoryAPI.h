#pragma once

#include <unordered_map>
#include "MemoryGlobals.h"
#include "MemoryDomains.h"

#include "Heap.h"
#include "LinearHeap.h"

//void* operator new(size_t size);
//void operator delete(void* p);

namespace memory
{
	/*
		MemoryManager is a singleton class that handles the creation and
		destruction of all primary memory domains.

		The idea is that MemoryManager uses malloc for a large portion of
		memory at the beginning of the application, where all the future
		allocations happens inside this pre-allocated memory.
	*/
	class MemoryManager
	{
	public:

		// Delete copy constructor and copy operator
		MemoryManager(const MemoryManager& other) = delete;
		MemoryManager& operator=(const MemoryManager& other) = delete;

		static MemoryManager& instance();
		static void end();

	private:

		MemoryManager();
		~MemoryManager();

		static MemoryManager* mInstance;

		void *mpHeapStart;
		const uint mHeapSize;
		heaps::LinearHeap mHeap;

		std::unordered_map<MemoryDomain, heaps::Heap*> mDomains;

		/*
			Internal help functions
		*/
		static uint sumAllDomainSizes()
		{
			uint sum = 0;
			for (std::pair<MemoryDomain, DomainInfo> info : MEMORY_DOMAIN_SIZES)
			{
				sum += info.second.size;
			}
			return sum;
		}
	};
}