#pragma once

#include "MemoryGlobals.h"
#include "Allocator.h"

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
				Reserves a chunk of memory and returns a pointer to it. Returns nullptr if allocation
				isn't possible.
			*/
			void* Allocate(uint size) override;

			/*
				Returns a chunk of reserved memory back to the allocator, so that the memory can be used
				in future allocations.
			*/
			void Free(void* ptr) override;

			/*
			
			*/
			template <typename T>
			T* CreateSubAllocator(uint memorySize);

		private:

			void* mpCurrent;
		};
	}
}