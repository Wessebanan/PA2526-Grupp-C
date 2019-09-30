#pragma once

#include "MemoryGlobals.h"
#include "Allocator.h"



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
				Specifies the memory the allocator will manage. Set memoryIncludesAllocator to true
				if the allocator itself is included in the given memory, as it will offset the memory
				start with the size of the allocator.
			*/
			bool Initialize(void* memoryStart, uint memorySize, bool memoryIncludesAllocator = false) override;

			/*
				Hard reset allocator
			*/
			void Wipe() override;

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

		private:

			void* mpCurrent;
		};
	}
}