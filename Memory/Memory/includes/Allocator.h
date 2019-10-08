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

	TODO: Describe how allocators work internally.

																				MB: Memory Block (aka. allocations)
	+------------------+------------------------------------------------------------------------------------------+
	|    Allocator     |     MB    |              MB             |     MB     |      MB      |  MB  |       MB    |
	+------------------+------------------------------------------------------------------------------------------+
	\________ ________/\____________________________________________ ____________________________________________/
			 V                                                      V
	   HEADER BLOCK                                         ALLOCATION BLOCK
(where allocator is stored)                            (managed by the allocator)

	\_____________________________________________________ _______________________________________________________/
														  V
														 HEAP
*/

namespace memory
{
	namespace allocators
	{
		/*
			Allocator is a generic handle for all types of allocators. Allocators handles reservations
			in memory blocks.
		*/
		class Allocator
		{
		public:
			DENY_COPY(Allocator)

			Allocator() : mMemorySize(0), mMemoryUsed(0), mpMemoryStart(nullptr) {}
			virtual ~Allocator() {}

			virtual bool Initialize(void* memoryStart, uint memorySize) = 0;

			/*
				Clears all internal data in memory block. In order to use the allocator again,
				Initialize() has to be called.
			*/
			virtual void Terminate() { mMemorySize = 0; mMemoryUsed = 0; mpMemoryStart = nullptr; }

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
			uint mMemorySize;
			uint mMemoryUsed;

			// Pointer to the first address of the ALLOCATION BLOCK
			void* mpMemoryStart;
		};

	} // allocators
} // memory