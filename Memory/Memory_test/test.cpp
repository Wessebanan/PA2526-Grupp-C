#include "pch.h"
#include "MemoryAPI.h"

#include <crtdbg.h> // For checking memory leaks

int main(int argc, char** argv) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

/*
	In these unit tests, some expectations uses ASSERT_ instead of EXPECT_.
	When trying functionality that allocates memory, ASSERT_ are used to
	check the result as we don't want to continue the test if we don't get
	the memory we ask for.

	memory::Initialize()	<- Allocates memory using malloc
	memory::End()			<- deletes memory allocated through malloc
*/

namespace API
{
	const uint ALLOCATOR_SIZE = sizeof(memory::allocators::LinearAllocator);

	TEST(TestAPI, Init)
	{

		ASSERT_TRUE(memory::Initialize(100));
		memory::End();
	}

	TEST(TestAPI, CreateAllocator)
	{
		memory::Initialize(ALLOCATOR_SIZE + 50);

		memory::allocators::Allocator* p_allocator = memory::CreateAllocator(50);
		ASSERT_NE(p_allocator, nullptr);

		memory::End();
	}

	TEST(TestAPI, CreateMultipleAllocators)
	{
		/*
			Allocate for 10 allocators, so that each can hold 10 bytes
		*/
		memory::Initialize(ALLOCATOR_SIZE * 10 + 100);

		// 10 allocators should fit in 100 bytes, if they are 10 byte each
		for (int i = 0; i < 10; i++)
		{
			memory::allocators::Allocator* p_allocator = memory::CreateAllocator(10);
			ASSERT_NE(p_allocator, nullptr);
		}

		// The 11th allocator should not be created, as the memory is full
		memory::allocators::Allocator* p_allocator = memory::CreateAllocator(10);
		ASSERT_EQ(p_allocator, nullptr);

		memory::End();
	}

	TEST(TestAPI, HandleOverflowAllocatorSize)
	{
		memory::Initialize(100);

		memory::allocators::Allocator* p_allocator = memory::CreateAllocator(110);
		ASSERT_EQ(p_allocator, nullptr);

		memory::End();
	}
}