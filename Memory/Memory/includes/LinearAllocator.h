#pragma once

#include "MemoryGlobals.h"
#include "Allocator.h"

// Notation: Vector is used as a temporary freed memory data structure. Will be removed when backend is implemented.
#include <vector>

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
	\____________ ____________/\_______________________________________ _____________________________________/
	             V                                                     V
		  Allocator block								  Heap block (for allocations)
	\___________________________________________________ ___________________________________________________/
														V
												   Memory Block (for allocator)
*/

namespace memory
{
	namespace allocators
	{
		/*
							! NOTATION !

			This is a temporary class in order to create custom heaps
			using the memory API. This class will be updated in later
			pull-requests.
		*/
		class LinearAllocator : public Allocator
		{
		public:

			// Delete copy constructor and copy operator
			LinearAllocator(const LinearAllocator& other) = delete;
			LinearAllocator& operator=(const LinearAllocator& other) = delete;

			LinearAllocator();
			virtual ~LinearAllocator();

			/*
				Specifies the memory the allocator will manage. The allocator itself is stores as a header
				at the beginning of the memory if mananges, so the actal memory start will be offset by
				the size of the allocator.
			*/
			bool Initialize(void* memoryStart, uint memorySize) override;

			/*
				Clears all internal data in memory block. In order to use the allocator again,
				Initialize() has to be called.
			*/
			void Terminate() override;

			/*
				Clears all allocations in the heap block; freeing all allocated memory.
			*/
			void Clear() override;

			/*
				Reserves a block of memory in the allocator's memory heap, and returns a pointer to it.
				Returns nullptr if allocation failed.
			*/
			void* Allocate(uint size) override;

			/*
				Returns a block of reserved memory back to the allocator's memory heap, so that the memory
				can be used for future allocations.
			*/
			void Free(void* ptr) override;

			///*
			//	Reserves 
			//*/
			//template <typename T>
			//T* CreateSubAllocator(uint memorySize);

		private:

			void* mpCurrent;


			/*
				Notation, this is a temporary implementation to store freed memory blocks. This will
				be refactored in the backend implementation.

				For now, we use a linked list for both free and allocated blocks; one list for each.
			*/
			struct MemBlock
			{
				MemBlock(void* memoryStart, uint memorySize) : start(memoryStart), size(memorySize), prev(nullptr), next(nullptr) {}
				void* start;
				uint size;
				MemBlock* prev;
				MemBlock* next;
			};
			MemBlock* mFreeMemBlocks_temp; // Sorted by MemBlock::start address
			MemBlock* mUsedMemBlocks_temp; // Unsorted
		};
	}
}