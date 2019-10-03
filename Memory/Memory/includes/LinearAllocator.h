#pragma once

#include "MemoryGlobals.h"
#include "Allocator.h"

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

		private:

			void* mpCurrent;

			//// TEMPORARY BACKEND START ////
			/*
				In order to avoid memory leaks. Store all allocations
				and free them in the free function and destructor.
			*/

			// pair<ptr,size>
			std::vector<std::pair<void*, uint>> mAllocations;

			///// TEMPORARY BACKEND END /////
		};
	}
}