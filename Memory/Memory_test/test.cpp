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
			/*
				-- Initialize
				Try to initialize an allocator. This is necessary in order to use
				it later.
				Notice that an allocater does not allocate memory itself; it will
				get some memory to work with.
			*/
			TEST(TestFreeListAllocator, Initialize)
			{
				const uint memory_size = 100;
				void* p_memory_start = malloc(memory_size);

				// Create an allocator to work with, and give it the memory
				memory::allocators::FreeListAllocator allocator;
				ASSERT_TRUE(allocator.Initialize(p_memory_start, memory_size));

				// Cleanup
				free(p_memory_start);
			}

			/*
				-- Allocate
				If you as a user want to reserve some memory in the memory an allocator
				manage, you do so through its Allocate() method.
			*/
			TEST(TestFreeListAllocator, Allocate)
			{
				/*
					Specify memory constraints. Allocate memory for 10 allocations, where
					also include the memory of the allocation headers. This is not what
					an end user will have to do, as there are probably a heap that the
					user work with that capsulates the allocator.
				*/	
				const uint ALLOCATION_SIZE = sizeof(int);
				const uint ALLOCATION_COUNT = 10;
				const uint MEMORY_SIZE = (ALLOCATION_SIZE + memory::allocators::FreeListAllocator::GetAllocationHeaderSize()) * ALLOCATION_COUNT;

				// Allocate the memory that the allocater will use
				void* p_memory_start = malloc(MEMORY_SIZE);

				// Create an allocator to work with, and give it the memory
				memory::allocators::FreeListAllocator allocator;
				allocator.Initialize(p_memory_start, MEMORY_SIZE);

				// Allocate 10 objects and give them values based on i
				int* p_allocations[ALLOCATION_COUNT];
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					p_allocations[i] = (int*)allocator.Allocate(ALLOCATION_SIZE);
					EXPECT_NE(p_allocations[i], nullptr);
					*p_allocations[i] = i;
				}

				// Check if all allocated objects has the correct value
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					EXPECT_EQ(*p_allocations[i], i);
				}

				// Cleanup
				free(p_memory_start);
			}

			/*
				-- Free
				If you as a user is donw working with your allocated memory, you can free
				it so the memory can be used for other allocations. Is is done with the Free()
				method, which takes in a pointer to the memory you want to free.
			*/
			TEST(TestFreeListAllocator, Free)
			{
				// Specify memory constraints.
				const uint ALLOCATION_SIZE = sizeof(int);
				const uint ALLOCATION_COUNT = 10;
				const uint MEMORY_SIZE = (ALLOCATION_SIZE + memory::allocators::FreeListAllocator::GetAllocationHeaderSize()) * ALLOCATION_COUNT;

				// Allocate the memory that the allocater will use
				void* p_memory_start = malloc(MEMORY_SIZE);

				// Create an allocator to work with, and give it the memory
				memory::allocators::FreeListAllocator allocator;
				allocator.Initialize(p_memory_start, MEMORY_SIZE);

				/*
					Allocate 10 objects and give them values.
					We write to the memory because if we get incorrect memory and write
					to it, we will notice out-of-bounds crash when we free it.
				*/
				int* p_allocations[ALLOCATION_COUNT];
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					p_allocations[i] = (int*)allocator.Allocate(ALLOCATION_SIZE);
					*p_allocations[i] = i;
				}

				// Free all allocations, but in reverse order to make the free list non linear
				for (int i = ALLOCATION_COUNT-1; i >= 0; i--)
				{
					allocator.Free(p_allocations[i]);
				}

				// Try to allocate 10 objects again to see if the memory became free from Freeing it
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					p_allocations[i] = (int*)allocator.Allocate(ALLOCATION_SIZE);
					*p_allocations[i] = i;
				}

				// Check if all values of the new objets
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					EXPECT_EQ(*p_allocations[i], i);
				}

				// Cleanup
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					allocator.Free(p_allocations[i]);
				}
				free(p_memory_start);
			}

			TEST(TestFreeListAllocator, HandleOverflow)
			{
				// Specify memory constraints
				const uint ALLOCATION_SIZE = sizeof(int);
				const uint ALLOCATION_COUNT = 10;
				const uint MEMORY_SIZE = (ALLOCATION_SIZE + memory::allocators::FreeListAllocator::GetAllocationHeaderSize()) * ALLOCATION_COUNT;

				// Allocate the memory that the allocater will use
				void* p_memory_start = malloc(MEMORY_SIZE);

				// Create an allocator to work with, and give it the memory
				memory::allocators::FreeListAllocator allocator;
				allocator.Initialize(p_memory_start, MEMORY_SIZE);

				// 
				int* p_allocations[ALLOCATION_COUNT];
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					p_allocations[i] = (int*)allocator.Allocate(ALLOCATION_SIZE);
					*p_allocations[i] = i;
				}

				EXPECT_EQ(allocator.Allocate(ALLOCATION_SIZE), nullptr);

				// Cleanup
				free(p_memory_start);
			}

			TEST(TestFreeListAllocator, HandleFreeOutOfBounds)
			{
				// Specify memory constraints
				const uint ALLOCATION_SIZE = sizeof(int);
				const uint ALLOCATION_COUNT = 5;
				const uint ALLOCATION_WITH_HEADER_SIZE = ALLOCATION_SIZE + memory::allocators::FreeListAllocator::GetAllocationHeaderSize();

				// We want to allocate 5 objects, but we reserve size for 10 objects
				const uint MEMORY_SIZE = ALLOCATION_WITH_HEADER_SIZE * (ALLOCATION_COUNT*2);

				// Allocate the memory that the allocater will use
				void* p_memory_start = malloc(MEMORY_SIZE);

				// Create an allocator to work with, and give it the memory
				memory::allocators::FreeListAllocator allocator;
				allocator.Initialize(p_memory_start, MEMORY_SIZE);

				/*
					Allocate 5 objects, we don't fill the memory in order to be able to free
					memory that the allocator manage but is not reserved.
				*/
				void* p_last_allocation;
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					p_last_allocation = allocator.Allocate(ALLOCATION_SIZE);
				}

				// Calculate address that would be the next allocation, right after the last allocation.
				void* p_non_existing_allocation = (void*)((char*)p_last_allocation + ALLOCATION_WITH_HEADER_SIZE);

				// Try to free nullptr, should not break
				EXPECT_NO_THROW(allocator.Free(nullptr));

				// Try to free memory that is not reserved, should not break
				EXPECT_NO_THROW(allocator.Free(p_non_existing_allocation));

				// Cleanup
				free(p_memory_start);
			}

			TEST(TestFreeListAllocator, Getters)
			{
				// Specify memory constraints
				const uint ALLOCATION_SIZE = sizeof(int);
				const uint ALLOCATION_COUNT = 10;
				const uint ALLOCATION_WITH_HEADER_SIZE = ALLOCATION_SIZE + memory::allocators::FreeListAllocator::GetAllocationHeaderSize();
				const uint MEMORY_SIZE = ALLOCATION_WITH_HEADER_SIZE * ALLOCATION_COUNT;

				// Allocate the memory that the allocater will use
				void* p_memory_start = malloc(MEMORY_SIZE);

				// Create an allocator to work with, check the getters before initializing it
				memory::allocators::FreeListAllocator allocator;

				EXPECT_EQ(allocator.GetMemorySize(), 0);
				EXPECT_EQ(allocator.GetMemoryUsed(), 0);

				// Give the allocator the memory to work with
				allocator.Initialize(p_memory_start, MEMORY_SIZE);

				EXPECT_EQ(allocator.GetMemorySize(), MEMORY_SIZE);
				EXPECT_EQ(allocator.GetMemoryUsed(), 0);

				// Allocate 10 objects and check if count getter is correct for each allocation
				uint expected_used_size = 0;
				void* p_allocations[ALLOCATION_COUNT] = { nullptr };
				for (uint i = 0; i < ALLOCATION_COUNT; i++)
				{
					p_allocations[i] = allocator.Allocate(ALLOCATION_SIZE);
					expected_used_size += ALLOCATION_WITH_HEADER_SIZE;
					EXPECT_EQ(allocator.GetMemoryUsed(), expected_used_size);
				}

				// Free half of the allocations and check if count getter is correct
				for (uint i = 0; i < ALLOCATION_COUNT / 2.f; i++)
				{
					allocator.Free(p_allocations[i]);
					expected_used_size -= ALLOCATION_WITH_HEADER_SIZE;
					EXPECT_EQ(allocator.GetMemoryUsed(), expected_used_size);
				}

				// Clear all allocations from memory and check if getters are correct
				allocator.Clear();
				EXPECT_EQ(allocator.GetMemorySize(), MEMORY_SIZE);
				EXPECT_EQ(allocator.GetMemoryUsed(), 0);

				// Clear all meta data in the allocator and check if getters are correct
				allocator.Terminate();
				EXPECT_EQ(allocator.GetMemorySize(), 0);
				EXPECT_EQ(allocator.GetMemoryUsed(), 0);

				// Cleanup
				free(p_memory_start);
			}
		}
	}

	
}