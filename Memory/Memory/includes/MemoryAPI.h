#pragma once

#include <unordered_map>
#include "MemoryGlobals.h"

#include "LinearAllocator.h"

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

		/*
			Returns an instance to the Memory Manager. Use this instance
			to create custom memory domains.
		*/
		static MemoryManager& Instance();

		/*
			Allocates main memory for the MemoryManager. This method has to
			be called before using the manager to create allocators or allocate
			memory.
		*/
		bool Initialize(uint size);

		/*
			Frees all allocated memory.
		*/
		static void End();

		/*
			!BETA METHOD, LATER IMPLEMENTATIONS WILL SPECIFY ALLOCATOR TYPE!
			Creates an allocator for a new memory domain that the user can use.
			Memory domains are just different chunks in memory, reserved for
			one area of the application; like graphics or ECS.
		*/
		allocators::Allocator* CreateAllocator(uint size);

	private:

		MemoryManager();
		~MemoryManager();

		static MemoryManager* mInstance;

		uint mHeapSize;
		void *mpHeapStart;
		allocators::LinearAllocator mHeap;
	};
}