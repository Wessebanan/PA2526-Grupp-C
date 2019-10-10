#include "pch.h"
#include "MemoryAPI.h"

#include <crtdbg.h> // For checking memory leaks

//#pragma warning(disable:)

int main(int argc, char** argv) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

/*
												__STRUCTURE OF HEAPS__
						!! For more information about heaps, please read explanation in Heap.h !!

	Memory layout:																	  MB: Memory Block (aka. allocations)
	+------------------------+------------------------------------------------------------------------------------------+
	|   Heap   |  Allocator  |  MB    |              MB             |     MB     |      MB      |     MB    |     MB    |
	+------------------------+------------------------------------------------------------------------------------------+
	\___________ ___________/\____________________________________________ _____________________________________________/
				V                                                         V
		  HEADER BLOCK                                           ALLOCATION BLOCK
															(managed by the allocator)

	\________________________________________________________ __________________________________________________________/
															 V
														   HEAP


	REPRESENTAION FOR HOW THE MEMORY MANAGER IS USED IN AN APPLICATION:

  START
	V
	+-> memory::Initialize(<requested main heap size>)		<-- Creates a memory manager instance and initializes it.
	|															The manager uses malloc to allocate memory for the size
	|															of the wanted heap, plus the size of the allocator that
	|															manages it. The main allocator is then given the
	|															allocated memory.
	+-( From here on, users can use the memory API )
	|
	+-> memory::CreateHeap(<requested heap size>)			<-- Returns an sub-heap that lives in the main heap, that
	|															the user can use for their own dynamic data. This
	|															heap can itself contain sub-heaps.
	.
	.
	+-( Users application runs )
	.
	.
	|
	+-> memory::End()										<-- Deletes the memory manager instance. Then this happends,
	|															the manager will free all allocated memory.
	+-( From here on, users can not use the memory API )
	|
	V
   END
*/


/*
	NOTATION FOR ALL MEMORY TESTS:

	- All tests that uses the memory manager uses memory::Initialize() at the
	  beginning of the test in order to create a memory manager to work with.

	- Since memory manager is singleton, all tests that uses the memory manager
	  have responsibility of calling memory::End() at the end of the test in
	  order to delete the used instance.
*/


namespace API
{
	/*
		In order to use memory::Initialize(), we have to make sure that the
		underlying functionality works correctly. This function will fetch
		the memory manager instance and call Initialize() on it (so it's
		only easy usabiltiy).

		This test will try to work with the instance directly. Later tests
		will use the shortcut static methods.
	*/
	TEST(MemoryAPI, InitMemoryManager)
	{
		memory::MemoryManager& r_mgr = memory::MemoryManager::Instance();

		/*
			Initialize the manager with some memory size requirement,
			this will allocate memory using malloc.
			If failed, we don't want to free the memory; hince ASSERT
		*/
		ASSERT_TRUE(r_mgr.Initialize(100));

		// Frees memory
		r_mgr.End();
	}

	/*
		When the user want some reserved memory for their domain, like physics
		or ECS, they can reserve a sub-heap on the main heap. They can allocate
		and free memory on their own sub-heap, knowing it will never interfere
		with other user domains.

		As heaps will live in other heaps, with the main heap being the highest
		level, we let heaps be a factory for their sub-heaps. This test will
		try to create a sub-heap on the main heap.

		As there are now way of really knowing that heaps are freed outside
		the free function (same as for delete), it is 'tested' in the same
		test as the creation. If no memory leaks are detected, the test
		passes.
	*/
	TEST(MemoryAPI, CreateAndFreeSubHeap)
	{
		const uint MAIN_HEAP_SIZE = 500;
		const uint SUB_HEAP_SIZE = 100;

		memory::Initialize(MAIN_HEAP_SIZE);

		// Create a sub-heap and sanity check its creation.
		memory::Heap* p_heap = memory::CreateHeap(SUB_HEAP_SIZE);
		ASSERT_NE(p_heap, nullptr);

		// Free memory to avoid memory leaks.
		memory::Free(p_heap);
		memory::End();
	}

	/*
		If a user want to reserve memory for dynamic objects, they do so
		on a heap. This can be on the main-heap, but should be avoided as
		this heap is mostly reserved for sub-heap for the different domains
		in the application.

		Sub-heaps are good practice to do the allocations on. Let's test that!
		(As an allocation on the main-heap is done when we create a sub-heap,
		which is tested in another test.)

		As there are now way of really knowing that memory are freed outside
		the free function (same as for delete), it is 'tested' in the same
		test as the allocatio. If no memory leaks are detected, the test
		passes.
	*/
	TEST(MemoryAPI, AllocateAndFreeOnHeap)
	{
		const uint MAIN_HEAP_SIZE = 500;
		const uint SUB_HEAP_SIZE = 100;
		const uint OBJ_SIZE = 10;

		memory::Initialize(MAIN_HEAP_SIZE);

		memory::Heap* p_heap = memory::CreateHeap(SUB_HEAP_SIZE);

		// Try to allocate on the sub-heap and sanity check the returning ptr.
		void* p_allocation = p_heap->Allocate(OBJ_SIZE);
		ASSERT_NE(p_allocation, nullptr);

		// Free all memory to avoid memory leaks.
		p_heap->Free(p_allocation);
		memory::Free(p_heap);
		memory::End();
	}

	/*
		Check if an allocation that is larger than the heap is handled correctly
		and does not break.
	*/
	TEST(MemoryAPI, AllocateToMuch)
	{
		const uint MAIN_HEAP_SIZE = 500;
		const uint SUB_HEAP_SIZE = 100;
		const uint OBJ_SIZE = 300;

		memory::Initialize(MAIN_HEAP_SIZE);

		memory::Heap* p_heap = memory::CreateHeap(SUB_HEAP_SIZE);

		// Try to allocate on the sub-heap.
		void* p_allocation = p_heap->Allocate(OBJ_SIZE);
		EXPECT_EQ(p_allocation, nullptr);

		// Free all memory to avoid memory leaks.
		memory::Free(p_heap);
		memory::End();
	}

	/*
		Check if possible to free an already freed pointer
	*/
	TEST(MemoryAPI, FreeTwice)
	{
		const uint MAIN_HEAP_SIZE = 500;
		const uint SUB_HEAP_SIZE = 100;
		const uint OBJ_SIZE = 10;

		memory::Initialize(MAIN_HEAP_SIZE);

		memory::Heap* p_heap = memory::CreateHeap(SUB_HEAP_SIZE);
		void* p_allocation = p_heap->Allocate(OBJ_SIZE);

		// Free pointer
		p_heap->Free(p_allocation);

		// Try to free pointer again
		EXPECT_NO_THROW(p_heap->Free(p_allocation));

		// Free all memory to avoid memory leaks.
		memory::Free(p_heap);
		memory::End();
	}

	/*
		Heaps offer the functionality of allocating an array of objects. This makes it both
		easier for a user to access multiple objects and will save some memory overhead for
		allocations in the allocator header.
	*/
	TEST(MemoryAPI, AllocateArray)
	{
		const uint MAIN_HEAP_SIZE = 500;
		const uint SUB_HEAP_SIZE = 100;
		const uint OBJ_SIZE = 10;
		const uint OBJ_COUNT = 5;
		uint expected_values[OBJ_COUNT];

		memory::Initialize(MAIN_HEAP_SIZE);
		memory::Heap* p_heap = memory::CreateHeap(SUB_HEAP_SIZE);

		// Allocate array and check returning pointer
		int* p_array = p_heap->AllocateArray<int>(OBJ_COUNT);
		EXPECT_NE(p_array, nullptr);

		// Try to use array
		for (uint i = 0; i < OBJ_COUNT; i++)
		{
			p_array[i] = expected_values[i] = i;
		}

		// Check if values exist
		for (uint i = 0; i < OBJ_COUNT; i++)
		{
			EXPECT_EQ(p_array[i], expected_values[i]);
		}

		memory::Free(p_heap);
		memory::End();
	}

	TEST(MemoryAPI, FreeArray)
	{
		const uint MAIN_HEAP_SIZE = 500;
		const uint SUB_HEAP_SIZE = 100;
		const uint OBJ_SIZE = 10;
		const uint OBJ_COUNT = 5;

		memory::Initialize(MAIN_HEAP_SIZE);
		memory::Heap* p_heap = memory::CreateHeap(SUB_HEAP_SIZE);
		int* p_array = p_heap->AllocateArray<int>(OBJ_COUNT);

		for (int i = 0; i < OBJ_COUNT; i++)
		{
			p_array[i] = i;
		}

		// Try to free array from heap
		EXPECT_NO_THROW(p_heap->Free(p_array));

		memory::Free(p_heap);
		memory::End();
	}
}

namespace Backend
{
	namespace Allocators
	{
		namespace FreeListAllocator
		{
			TEST(TestFreeListAllocator, Initialize)
			{
				const uint memory_size = 100;

				void* p_memory_start = malloc(memory_size);

				memory::allocators::FreeListAllocator allocator;
				ASSERT_TRUE(allocator.Initialize(p_memory_start, memory_size));

				free(p_memory_start);
			}

			TEST(TestFreeListAllocator, Allocate)
			{
				const uint ALLOCATION_SIZE = sizeof(int);
				const uint ALLOCATION_COUNT = 10;
				const uint MEMORY_SIZE = (ALLOCATION_SIZE + memory::allocators::FreeListAllocator::GetAllocationHeaderSize()) * ALLOCATION_COUNT;

				void* p_memory_start = malloc(MEMORY_SIZE);

				memory::allocators::FreeListAllocator allocator;
				allocator.Initialize(p_memory_start, MEMORY_SIZE);

				int* p_allocations[ALLOCATION_COUNT];
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					p_allocations[i] = (int*)allocator.Allocate(ALLOCATION_SIZE);
					EXPECT_NE(p_allocations[i], nullptr);
					*p_allocations[i] = i;
				}

				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					EXPECT_EQ(*p_allocations[i], i);
				}

				free(p_memory_start);
			}

			TEST(TestFreeListAllocator, Free)
			{
				const uint ALLOCATION_SIZE = sizeof(int);
				const uint ALLOCATION_COUNT = 10;
				const uint MEMORY_SIZE = (ALLOCATION_SIZE + memory::allocators::FreeListAllocator::GetAllocationHeaderSize()) * ALLOCATION_COUNT;

				void* p_memory_start = malloc(MEMORY_SIZE);

				memory::allocators::FreeListAllocator allocator;
				allocator.Initialize(p_memory_start, MEMORY_SIZE);

				int* p_allocations[ALLOCATION_COUNT];
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					p_allocations[i] = (int*)allocator.Allocate(ALLOCATION_SIZE);
					*p_allocations[i] = i;
				}

				for (int i = ALLOCATION_COUNT-1; i >= 0; i--)
				{
					allocator.Free(p_allocations[i]);
				}

				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					p_allocations[i] = (int*)allocator.Allocate(ALLOCATION_SIZE);
					*p_allocations[i] = i;
				}

				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					EXPECT_EQ(*p_allocations[i], i);
				}

				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					allocator.Free(p_allocations[i]);
				}

				free(p_memory_start);
			}

			TEST(TestFreeListAllocator, HandleOverflow)
			{
				const uint ALLOCATION_SIZE = sizeof(int);
				const uint ALLOCATION_COUNT = 10;
				const uint MEMORY_SIZE = (ALLOCATION_SIZE + memory::allocators::FreeListAllocator::GetAllocationHeaderSize()) * ALLOCATION_COUNT;

				void* p_memory_start = malloc(MEMORY_SIZE);

				memory::allocators::FreeListAllocator allocator;
				allocator.Initialize(p_memory_start, MEMORY_SIZE);

				int* p_allocations[ALLOCATION_COUNT];
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					p_allocations[i] = (int*)allocator.Allocate(ALLOCATION_SIZE);
					*p_allocations[i] = i;
				}

				EXPECT_EQ(allocator.Allocate(ALLOCATION_SIZE), nullptr);

				free(p_memory_start);
			}

			TEST(TestFreeListAllocator, HandleFreeOutOfBounds)
			{
				const uint ALLOCATION_SIZE = sizeof(int);
				const uint ALLOCATION_COUNT = 5;
				const uint ALLOCATION_WITH_HEADER_SIZE = ALLOCATION_SIZE + memory::allocators::FreeListAllocator::GetAllocationHeaderSize();
				const uint MEMORY_SIZE = ALLOCATION_WITH_HEADER_SIZE * (ALLOCATION_COUNT*2);

				void* p_memory_start = malloc(MEMORY_SIZE);

				memory::allocators::FreeListAllocator allocator;
				allocator.Initialize(p_memory_start, MEMORY_SIZE);
				void* p_last_allocation;
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					p_last_allocation = allocator.Allocate(ALLOCATION_SIZE);
				}

				void* p_non_existing_allocation = (void*)((char*)p_last_allocation + ALLOCATION_WITH_HEADER_SIZE);
				EXPECT_NO_THROW(allocator.Free(nullptr));
				EXPECT_NO_THROW(allocator.Free(p_non_existing_allocation));

				free(p_memory_start);
			}
		}
	}

	
}