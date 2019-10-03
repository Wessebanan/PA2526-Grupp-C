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
												__MAIN HEAP STRUCTURE__


	Allocator that manages                                           Memory Blocks (MB) that are allocated on the main heap. 
	the main heap, onwed by                                        	 These can include dara like dynamic variables, or sub-
	the memory manager                                               allocators with their own heap within the block.
	          |
	          V
	+------------------+------------------------------------------------------------------------------------------+
	| Allocator header |     MB    |              MB             |     MB     |      MB      |  MB  |       MB    |
	+------------------+------------------------------------------------------------------------------------------+
	\________ ________/\____________________________________________ ____________________________________________/
	         V                                                      V
	   Allocator Block                                          MAIN HEAP
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
										##################################
									   #  HOW ALLOCATOR MEMORY IS STORED  #
										##################################

	+-------------------------------------------------------------------------------------------------------+
	|  Header used to store   |                                                                             |
	|  the allocator and all  |			Memory available for allocations, managed by the allocator.			|
	|  data used to manage    |                                                                             |
	|  its memory.            |                                                                             |
	+-------------------------------------------------------------------------------------------------------+
	\____________ ____________/\_______________________________________ _____________________________________/
				 V                                                     V
		  Allocator block								  Heap block (for allocations)
	\___________________________________________________ ___________________________________________________/
														V
												   Memory Block (for allocator)
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
	TEST(MemoryAPI, InitMemoryManager)
	{
		memory::MemoryManager& r_mgr = memory::MemoryManager::Instance();
		//memory::MemoryManager::Instance().Initialize(100);

		/*
			Initialize the manager with some memory size requirement,
			this will allocate memory using malloc.
			If failed, we don't want to free the memory; hince ASSERT
		*/
		ASSERT_TRUE(r_mgr.Initialize(100));

		// Frees memory
		r_mgr.End();
	}

	//TEST(MemoryAPI, CreateHeap)
	//{
	//	const uint MAIN_HEAP_SIZE = 500;
	//	const uint SUB_HEAP_SIZE = 100;

	//	memory::Initialize(MAIN_HEAP_SIZE);

	//	memory::Heap* p_heap = memory::CreateHeap(SUB_HEAP_SIZE);

	//	ASSERT_NE(p_heap, nullptr);

	//	memory::Free(p_heap);
	//	memory::End();
	//}

	//TEST(MemoryAPI, Allocate)
	//{

	//}

	//TEST(MemoryAPI, Free)
	//{

	//}
}

/*
	TODO: Backend testing
*/