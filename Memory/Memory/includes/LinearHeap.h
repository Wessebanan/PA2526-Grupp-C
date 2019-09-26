#pragma once

#include "MemoryGlobals.h"
#include "Heap.h"

/*
							! NOTATION !

	This is a temporary class in order to create custom heaps
	using the memory API.
*/

namespace memory
{
	namespace heaps
	{
		class LinearHeap : public Heap
		{
		public:

			// Delete copy constructor and copy operator
			LinearHeap(const LinearHeap& other) = delete;
			LinearHeap& operator=(const LinearHeap& other) = delete;

			LinearHeap();
			virtual ~LinearHeap();

			bool initialize(void* memoryStart, uint memorySize) override;
			void* allocate(uint size) override;

		private:

			void* mMemoryStart;
			uint mMemorySize;
			uint mUsedMemory;

			void* mCurrent;
		};
	}
}