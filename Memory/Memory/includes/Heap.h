#pragma once

#include "MemoryGlobals.h"

namespace memory
{
	namespace heaps
	{
		template <typename T>
		struct HeapInitHandle
		{
			uint size;
			HeapInitHandle* parentHeap;
			
			void submitChild(uint childSize)
			{
				size += childSize;
				if (parentHeap)
				{
					parentHeap->submitChild(childSize);
				}
			}
		};

		class Heap
		{
		public:
			Heap(const Heap& other) = delete;
			Heap& operator=(const Heap& other) = delete;

			Heap() {}
			virtual ~Heap() {}

			virtual bool initialize(void* memoryStart, uint memorySize) = 0;
			virtual void* allocate(uint size) = 0;

			std::string heapString;

		private:
		};

	} // heaps
} // memory