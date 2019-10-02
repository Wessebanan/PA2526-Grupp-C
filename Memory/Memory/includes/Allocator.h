#pragma once

#include "MemoryGlobals.h"

/*
							! NOTATION !

	For this pull-request, there exist only only one class that
	inherits from Allocator; LinearAllocator. In future pull-
	requests, there will exist different types of allocators that
	each manage memory differently.
*/

/*
										##################################
									   #  HOW ALLOCATOR MEMORY IS STORED  #
										##################################

	+-------------------------------------------------------------------------------------------------------+
	|  Header used to store   |                                                                             |
	|  the allocator and all  |			Memory available for allocations, managed by the allocator.			|
	|  data used to manage    |                                                                             |
	|  its memory.            |                                                                             |
	+-------------------------------------------------------------------------------------------------------+
	                          \_______________________________________ _____________________________________/
							                                          V
	                                                              Heap block (for allocations)
	\___________________________________________________ ___________________________________________________/
	                                                    V
	                                               Memory Block (for allocator)
*/

namespace memory
{
	namespace allocators
	{
		/*
			Allocator is a generic handle for all types of allocators.
		*/
		class Allocator
		{
		public:
			Allocator(const Allocator& other) = delete;
			Allocator& operator=(const Allocator& other) = delete;

			Allocator() : mMemoryBlockSize(0), mMemoryHeapSize(0), mMemoryUsed(0), mpMemoryHeapStart(nullptr), mpMemoryBlockStart(nullptr) {}
			virtual ~Allocator() {}

			virtual bool Initialize(void* memoryStart, uint memorySize) = 0;

			/*
				Clears all internal data in memory block. In order to use the allocator again,
				Initialize() has to be called.
			*/
			virtual void Terminate() { mMemoryBlockSize = 0; mMemoryHeapSize = 0; mMemoryUsed = 0; mpMemoryHeapStart = nullptr; }

			/*
				Clears all allocations in the heap block; freeing all allocated memory.
			*/
			virtual void Clear() = 0;

			/*
				Reserves a block of memory in the allocator's memory heap, and returns a pointer to it.
				Returns nullptr if allocation failed.
			*/
			virtual void* Allocate(uint size) = 0;

			/*
				Returns a block of reserved memory back to the allocator's memory heap, so that the memory
				can be used for future allocations.
			*/
			virtual void Free(void* ptr) = 0;


		protected:

			// Total size of the memory block
			uint mMemoryBlockSize;
			uint mMemoryHeapSize;
			uint mMemoryUsed;

			/*
				Where the memory block for allocations start.
				(which is right after where the allocator for this block is stored)
			*/
			void* mpMemoryHeapStart;			

			/*
				Start of the given memory block the allocator manages. The allocator is
				stored at the beginning of this memory, followed by the memory block
				available for allocations.
			*/
			void* mpMemoryBlockStart;	
		};

	} // allocators
} // memory