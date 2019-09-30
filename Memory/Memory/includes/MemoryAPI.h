#pragma once

#include <unordered_map>
#include "MemoryGlobals.h"

#include "Heap.h"
#include "LinearHeap.h"



namespace memory
{
	using AllocatorType = unsigned int;
	enum AllocatorTypes
	{
		Linear,

		Undefined
	};

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

		static MemoryManager& Instance();

		bool Initialize(uint size);
		static void End();

		allocators::Allocator* CreateHeap(uint size);

	private:

		MemoryManager();
		~MemoryManager();

		static MemoryManager* mInstance;

		uint mHeapSize;
		void *mpHeapStart;
		allocators::LinearAllocator mHeap;
	};
}