#pragma once

#include "MemoryGlobals.h"
#include "LinearAllocator.h"

/*
												! HEAP VISUALISATION !

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
	blocks for the user (an exception for this is creating another heap, as they has to be created from a heap).
	A memory block can for example contain another heap, that only manage memory within that block.
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

																				      MB: Memory Block (aka. allocations)
	+------------------------+------------------------------------------------------------------------------------------+
	|   Heap   |  Allocator  |  MB    |              MB             |     MB     |      MB      |     MB    |     MB    |
	+------------------------+------------------------------------------------------------------------------------------+
	\___________ ___________/\____________________________________________ _____________________________________________/
	            V                                                         V
	      HEADER BLOCK                                           ALLOCATION BLOCK
                                                            (managed by the allocator)

	\________________________________________________________ __________________________________________________________/
	                                                         V
                                                           HEAP
*/

namespace memory
{
	/*
		A heap is the custom memory interface for a user. On a heap, a user can reserve memory by using Allocate(),
		and free it through Free(). A heap can also contain a sub-heap, which is created by using CreateHeap().
		A heap will automatically keep track of free and used memory for the user.
	*/
	class Heap
	{
	public:
		DENY_COPY(Heap)

		/*
			Reserves memory of given size on the heap, and returns a pointer to it.
		*/
		void* Allocate(uint size);

		/*
			Frees reserved memory on the heap. The parameter pointer has to be previously
			allocated on the same heap Free() are called on.
		*/
		template <typename T>
		void Free(T* pObject);

		/*
			Reserves memory for a sub-heap within the heap. The size of the reserved memory
			is the given size plus the size of the heap's header (size of Heap and its allocator).
			In memory, this will look like:
			|///////| Sub-Heap | Allocator | The heaps allocatable memory |//////////////////////|

			(/// = other memory allocations.)
		*/
		Heap* CreateHeap(uint size);

	private:
		Heap();
		~Heap();

		/*
			A heap can't be used before it's initialized, which is done by giving
			it a pointer to the memory it will work with and the size of that memory.
		*/
		bool Initialize(void* memoryStart, uint memorySize);

		/*
			Clears all internal data in the heap.
		*/
		void Terminate();

		void* mpAllocationBlockStart;
		void* mpMemoryStart;

		/*
			Since heaps will use a different types of allocator depending of how the user want
			to work with the memorys (fixed size objects, varying size objects, no fragmentation etc.)
			the allocator is stored as a parent pointer.
		*/
		allocators::Allocator* mpAllocator;

		friend class MemoryManager;
		friend class Heap;
	};


	/*
		Templated heap methods. Must be in header, as templated types isn't known at compile time.
	*/
	template <typename T>
	void memory::Heap::Free(T* pObject)
	{
		mpAllocator->Free(pObject);
	}
}