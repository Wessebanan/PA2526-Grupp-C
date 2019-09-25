#pragma once

#include <unordered_map>
#include "LinearHeap.h"

//void* operator new(size_t size);
//void operator delete(void* p);

namespace memory
{

	

	

	class MemoryManager
	{
	public:

		// Delete copy constructor and copy operator
		MemoryManager(const MemoryManager& other) = delete;
		MemoryManager& operator=(const MemoryManager& other) = delete;

		static MemoryManager& getInstance();
		~MemoryManager();



	private:

		MemoryManager();

		static MemoryManager* mInstance;

		void *mpHeapStart;
		LinearHeap mHeap;
	};
}