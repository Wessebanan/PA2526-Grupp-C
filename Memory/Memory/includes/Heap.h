#pragma once

#include "MemoryGlobals.h"
#include "LinearAllocator.h"

/*
												! HEAP VISUALISATION !

	MB: Memory Block

	-- Summary
	Allocating memory with 'new' uses system calls to reserve memory, which is what we want to avoid as it can be
	quite compute heavy. Therefore, Heaps provides custom allocations on a pre-allocated memory. If a user want to
	use dynamic memory, they do so through a heap.

	-- Creation
	Heaps can only be created from other heaps. When requesting a heap creation, the user request for a size of
	allocatable memory (ALLOCATION BLOCK). However, the heap needs a header in order to store memory management
	data. The size of the this header depends on what type of heap that is requested, and the allocation for the
	heap need to concider this header size, which is why creation of heaps are factored for the user.

	-- Allocations
	A user can allocate a memory block of given size from a heap. The heap reserves the block in its ALLOCATION BLOCK
	and returns pointer to it.

	-- Freeing memory
	A user can free memory by giving heap a pointer to the allocated memory, so that the memory can be used in future
	allocations. Memory can only be freed by the heap that allocated it.

	Memory blocks can contain anything, it's not up to the heap to fill the block with data; it only provides memory
	blocks for the user. A memory block can for example contain another heap, that only manage memory within that block.
	(insert inception meme; brain explosions etc.)

	-- Notations
	! The size of a heap can't be changed.
	! The user is responsible for not work out-of-bounds of the allocated memory. This principle is the same as for
	  the everyday 'new' operator.
	! When requesting a heap creation, the user request the size of the ALLOCAITON BLOCK (see below). The size of
	  the total memory allocated for the heap will add the size fo the allocator used, in order to store it at the
	  beginning of its memory.

	-- Management
	There exist different types of allocators that each manage memory differently. Some allows fragmentation, some
	does not for example.
	TOOD: Write some information about allocators.

	Main functionality of heaps from a user's perspective:
		- Allocate()	:	Reserves a Memory Block and returns a pointer to it.
		- Free()		:	Takes a pointer and releases its memory reservation.
							The pointer must come from the same heap.

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
	namespace heaps
	{
		class Heap
		{
		public:
			DENY_COPY(Heap)

			void* Allocate(uint size);
			void Free(void* ptr);

			// TODO: Create heap

		private:
			Heap();
			~Heap();

			bool Initialize(void* memoryStart, uint memorySize);

			void* mpAllocationBlockStart;
			void* mpMemoryStart;
			allocators::LinearAllocator* mpAllocator;

			friend class MemoryManager;
		};
	}
}