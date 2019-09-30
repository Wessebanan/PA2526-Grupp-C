#pragma once

#include "MemoryGlobals.h"
#include "Allocator.h"

/*
							! NOTATION !

	This is a temporary class in order to create custom heaps
	using the memory API. This class will be updated in later
	pull-requests.
*/

namespace memory
{
	namespace allocators
	{
		class LinearAllocator : public Allocator
		{
		public:

			// Delete copy constructor and copy operator
			LinearAllocator(const LinearAllocator& other) = delete;
			LinearAllocator& operator=(const LinearAllocator& other) = delete;

			LinearAllocator();
			virtual ~LinearAllocator();

			bool Initialize(void* memoryStart, uint memorySize, bool memoryIncludesAllocator = false) override;

			void* Allocate(uint size) override;
			void Free(void* ptr) override;

		private:

			void* mCurrent;
		};
	}
}