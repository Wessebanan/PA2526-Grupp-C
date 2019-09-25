#pragma once

#include "MemoryGlobals.h"

/*
							! NOTATION !

	This is a temporary class in order to create custom heaps
	using the memory API.
*/

namespace memory
{


	class LinearHeap
	{
	public:

		// Delete copy constructor and copy operator
		LinearHeap(const LinearHeap& other) = delete;
		LinearHeap& operator=(const LinearHeap& other) = delete;

		LinearHeap(void *memoryStart, uint memorySize);
		~LinearHeap();

	private:

		void* mMemoryStart;
		uint mMemorySize;
	};
}