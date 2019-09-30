#pragma once

#include "MemoryGlobals.h"

/*
							! NOTATION !

	For this pull-request, there exist only only one class that
	inherits from Allocator; LinearAllocator. In future pull-
	requests, there will exist different types of allocators that
	each manage memory differently.
*/

namespace memory
{
	namespace allocators
	{
		/*
			Allocator is a generic handle for all types of allocators.
		*/
		class Allocator
		{
		public:
			Allocator(const Allocator& other) = delete;
			Allocator& operator=(const Allocator& other) = delete;

			Allocator() : mMemorySize(0), mMemoryUsed(0), mpMemoryStart(nullptr) {}
			virtual ~Allocator() {}

			virtual bool Initialize(void* memoryStart, uint memorySize, bool memoryIncludesAllocator = false) = 0;

			virtual void* Allocate(uint size) = 0;
			virtual void Free(void* ptr) = 0;

		protected:

			uint mMemorySize;
			uint mMemoryUsed;
			void* mpMemoryStart;
		};

	} // allocators
} // memory