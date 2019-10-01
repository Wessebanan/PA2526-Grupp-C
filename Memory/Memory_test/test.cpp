#include "pch.h"
#include "MemoryAPI.h"

#include <crtdbg.h> // For checking memory leaks

int main(int argc, char** argv) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

/*
	
	Allocator that manages                                           Memory Blocks (MB) that are allocated on the main heap. 
	the main heap, onwed by                                        	 These can include dara like dynamic variables, or sub-
	the memory manager                                               allocators with their own heap within the block.
	          |
	          V
	+------------------+------------------------------------------------------------------------------------------+
	| Allocator header |     MB    |              MB             |     MB     |      MB      |  MB  |       MB    |
	+------------------+------------------------------------------------------------------------------------------+
	                   \____________________________________________ ____________________________________________/
	                                                                V
	                                                            MAIN HEAP
	                                       (managed by an allocator owned by the memory manager)

	\_____________________________________________________ _______________________________________________________/
	                                                      V
	                                             All allocated memory
	


	VISAUL REPRESENTAION FOR HOW THE MEMORY MANAGER IS USED IN AN APPLICATION:

  START
	V
	+-> memory::Initialize(<requested main heap size>)		<-- Creates a memory manager instance and initializes it.
	|															The manager uses malloc to allocate memory for the size
	|															of the wanted heap, plus the size of the allocator that
	|															manages it. The main allocator is then given the
	|															allocated memory.
	+-( From here on, users can use the memory API )
	|
	+-> memory::CreateAllocator(<requested heap size>)		<-- Returns an sub-allocator that the user can use for their
	|															own dynamic data. This allocator can itself contain sub-
	|															allocators.
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
		TESTING API

	*/

	const uint ALLOCATOR_SIZE = sizeof(memory::allocators::LinearAllocator);

	// Let there be some data the user want to use
	const uint user_data_size = 50;

	/*
		Try to initialize memory.
		This will create an instance of memory manager and initialize it with the
		wanted heap size. The memory manager will use malloc for the wanted heap
		size plus the size of the allocator that manages the main heap.

		Initialize() returns true if malloc returns null, or the main allocator
		failed to initialize.
	*/
	TEST(TestAPI, Init)
	{
		ASSERT_TRUE(memory::Initialize(100));
		memory::End();
	}

	/*
		Try to create an allocator that lives on the main heap and manages a memory
		block on the main heap.
	*/
	TEST(TestAPI, CreateAllocator)
	{
		// Request a heap that fits an allocator and user data
		memory::Initialize(ALLOCATOR_SIZE + user_data_size);

		// Create an allocator that the user can use to allocate memory for their data
		memory::allocators::Allocator* p_allocator = memory::CreateAllocator(user_data_size);
		ASSERT_NE(p_allocator, nullptr);

		// Free allocated memory
		memory::End();
	}

	/*
		In an application, there will probably exist multiple allocators on the main heap
		that each manage a block of memory separately. Graphics, ECS, Sound etc. want their
		own block of data for memory allocations. These will recieve their own allocator for
		that purpose.
	*/
	TEST(TestAPI, CreateMultipleAllocators)
	{
		const uint domain_count = 10;

		// Request a heap that fits ten allocator and user data for each of them
		memory::Initialize
		(
			(ALLOCATOR_SIZE * domain_count) +
			(user_data_size * domain_count)
		);


		// Try to create each domain allocator, with their own heap size for user data
		for (uint i = 0; i < domain_count; i++)
		{
			memory::allocators::Allocator* p_allocator = memory::CreateAllocator(user_data_size);
			ASSERT_NE(p_allocator, nullptr);
		}

		/*
			Since we created allocators that fill the whole main heap, we should not be able to
			create another allocator.
		*/
		memory::allocators::Allocator* p_allocator = memory::CreateAllocator(user_data_size);
		ASSERT_EQ(p_allocator, nullptr);

		// Free allocated memory
		memory::End();
	}

	/*
		Each allocator is responsible for not allocating memory outside of their own heap.
	*/
	TEST(TestAPI, HandleOverflowAllocatorSize)
	{
		// Request a heap that fits an allocator and user data
		memory::Initialize(ALLOCATOR_SIZE + user_data_size);

		/*
			Try to create an allocator with a heap LARGER than we previously allocated memory for,
			which should not be allowed.
		*/
		memory::allocators::Allocator* p_allocator = memory::CreateAllocator(user_data_size + 10);
		ASSERT_EQ(p_allocator, nullptr);

		// Free allocated memory
		memory::End();
	}
}