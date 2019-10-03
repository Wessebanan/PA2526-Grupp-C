#pragma once

#include "MemoryGlobals.h"

namespace memory
{
	namespace heaps
	{
		class Heap
		{
		public:
			DENY_COPY(Heap)

			virtual void* Allocate(uint size) = 0;
			virtual void Free(void* ptr) = 0;

			// TODO: Create heap

		protected:
			Heap();
			~Heap();
		};
	}
}