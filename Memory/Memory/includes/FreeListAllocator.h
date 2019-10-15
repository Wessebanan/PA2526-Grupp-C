#pragma once

#include "MemoryGlobals.h"
#include "Allocator.h"
#include <new>

/*
										##################################
									   #  HOW ALLOCATOR MEMORY IS STORED  #
										##################################

	TODO: Describe how allocators work internally.

																				MB: Memory Block (aka. allocations)
	+------------------+------------------------------------------------------------------------------------------+
	|    Allocator     |     MB    |              MB             |     MB     |      MB      |  MB  |       MB    |
	+------------------+------------------------------------------------------------------------------------------+
	\________ ________/\____________________________________________ ____________________________________________/
			 V                                                      V
	   HEADER BLOCK                                         ALLOCATION BLOCK
(where allocator is stored)                            (managed by the allocator)

	\_____________________________________________________ _______________________________________________________/
														  V

*/
namespace memory
{
	namespace allocators
	{

		class FreeListAllocator : public Allocator
		{
		public:

			// Delete copy constructor and copy operator
			DENY_COPY(FreeListAllocator)

			FreeListAllocator();
			virtual ~FreeListAllocator();

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
				Reserves a block of memory in the allocator's memory, and returns a pointer to it.
				Returns nullptr if allocation failed.
			*/
			void* Allocate(uint size) override;

			/*
				Returns a block of reserved memory back to the allocator's memory, so that the memory
				can be used for future allocations.
			*/
			void Free(void* pObject) override;

			/*
				Getters
			*/

			// Returns the size of the header that is allocated for each allocation.
			inline static uint GetAllocationHeaderSize()
			{
				return sizeof(AllocationHeader);
			}

		private:

			/*
				As every allocation can differ in size, each allocation need an header that
				specify the size of the allocation. The size is needed when it's time to free
				the allocation.
			*/
			struct AllocationHeader
			{
				AllocationHeader(uint memorySize) : size(memorySize), padding(0) {}
				uint size;
				uint padding;
			};

			/*
				FreeBlocks are placed at the start of memory that is not reserved, so we know how large
				that gap of available memory is. FreeBlock also contain a pointer to the next available
				memory block; creating a linked list of free memory.
			*/
			struct FreeBlock
			{
				FreeBlock(uint memorySize, FreeBlock* pNextBlock = nullptr) : size(memorySize), pNext(pNextBlock) { }
				FreeBlock* pNext;
				uint size;
			};

			// Pointer to the first free memory block. If nullptr, the allocator is either full or not initialized.
			FreeBlock* mpFreeBlocks;



			/*
				Help methods
			*/

			/*
				Places and initializes an allocation header at given address, and
				returns a pointer to where the actual allocation starts (right
				after the header).
			*/
			inline void* MakeAllocationAt(void* pHeaderStart, uint allocationSize)
			{
				const uint HEADER_SIZE = sizeof(AllocationHeader);

				mMemoryUsed += allocationSize + HEADER_SIZE;
				new(pHeaderStart) AllocationHeader(allocationSize);
				return (void*)((char*)pHeaderStart + HEADER_SIZE);
			}

			/*
				Returns the pointer to the header holds the information about a
				given allocation.
			*/
			inline AllocationHeader* FetchHeader(void* pAllocation)
			{
				return (AllocationHeader*)((char*)pAllocation - sizeof(AllocationHeader));
			}

			/*
				Returns true if a given allocation is allocated by this allocator.
			*/
			inline bool IsAllocatedByMe(void* pAllocation)
			{
				const intptr_t START = reinterpret_cast<intptr_t>(mpMemoryStart);
				const intptr_t END = reinterpret_cast<intptr_t>((void*)((char*)mpMemoryStart + mMemorySize));
				const intptr_t ALLOCATION = reinterpret_cast<intptr_t>(pAllocation);

				if (ALLOCATION < START || ALLOCATION >= END)
				{
					return false;
				}
				return true;
			}
		};
	}
}