#include "pch.h"

#include <ecs.h>
#pragma comment(lib, "plainECS.lib")

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

namespace TestPoolAllocator
{
	// Memory information
	const unsigned int object_size = 10;
	const unsigned int object_count = 10;
	const unsigned int mem_size = object_size * object_count;

	TEST(TestPoolAllocator, Init)
	{
		// Allocate memory for 10 objects
		void* mem = malloc(mem_size);

		unsigned int first_address = (unsigned int)mem;
		unsigned int last_address = first_address + mem_size;

		// Create allocator
		PoolAllocator allocator;

		// Expect getters default values before initializing
		EXPECT_EQ(allocator.getAllocations(), 0);
		EXPECT_EQ(allocator.getUsedMemory(), 0);

		allocator.initialize(mem, mem_size, object_size);

		// Check getters, now after initialization
		EXPECT_EQ(allocator.getAllocations(), 0);
		EXPECT_EQ(allocator.getUsedMemory(), 0);

		free(mem);
	}
	TEST(TestPoolAllocator, Allocation)
	{
		// Allocate memory for 10 objects
		void* mem = malloc(mem_size);

		unsigned int first_address = (unsigned int)mem;
		unsigned int last_address = first_address + mem_size;

		// Create allocator
		PoolAllocator allocator;
		allocator.initialize(mem, mem_size, object_size);

		/*
			Try allocating 10 objects
			All these allocations should return a pointer address
			within the allocated memory.
		*/
		for (unsigned int i = 0; i < object_count; i++)
		{
			void* p = allocator.allocate();

			// Check if allocated memory is within memory chunk
			EXPECT_GE((unsigned int)p, first_address);
			EXPECT_LT((unsigned int)p, last_address);

			// Check getters
			unsigned int expectedAllocations = i + 1;
			EXPECT_EQ(allocator.getAllocations(), expectedAllocations);
			EXPECT_EQ(allocator.getUsedMemory(), expectedAllocations * object_count);
		}

		free(mem);
	}
	TEST(TestPoolAllocator, AvoidOutOfRange)
	{
		// Fake object information
		const unsigned int object_size = 10;
		const unsigned int object_count = 10;
		const unsigned int mem_size = object_size * object_count;

		// Allocate memory for 10 objects
		void* mem = malloc(mem_size);

		// Create allocator
		PoolAllocator allocator;
		allocator.initialize(mem, mem_size, object_size);

		// Allocate 10 objects
		for (unsigned int i = 0; i < object_count; i++)
		{
			allocator.allocate();
		}

		// Expect no more allocations in pool, limit is reached
		EXPECT_EQ(allocator.allocate(), nullptr);

		free(mem);
	}
	TEST(TestPoolAllocator, AllocatingAfterDeallocation)
	{
		// Fake object information
		const unsigned int object_size = 10;
		const unsigned int object_count = 10;
		const unsigned int mem_size = object_size * object_count;

		const unsigned int deallocIndexList[] = { 9, 0, 8, 1, 7, 2, 6, 3, 5, 4 };

		// Allocate memory for 10 objects
		void* mem = malloc(mem_size);

		// Create allocator
		PoolAllocator allocator;
		allocator.initialize(mem, mem_size, object_size);

		// Create a list for allocated pointers
		void* pointer_list[object_count];

		// Try allocate all for objects
		for (unsigned int i = 0; i < object_count; i++)
		{
			pointer_list[i] = allocator.allocate();
		}

		// Deallocate in a different order to make a non-linear free list
		for (unsigned int i = 0; i < object_count; i++)
		{
			allocator.free(pointer_list[i]);
		}

		// Try allocate all for objects, again, and check return this time
		void* p;
		for (unsigned int i = 0; i < object_count; i++)
		{
			p = allocator.allocate();
			EXPECT_NE(p, nullptr);
		}

		free(mem);
	}
	TEST(TestPoolAllocator, AllocateAfterReset)
	{
		// Fake object information
		const unsigned int object_size = 10;
		const unsigned int object_count = 10;
		const unsigned int mem_size = object_size * object_count;

		// Allocate memory for 10 objects
		void* mem = malloc(mem_size);

		// Create allocator
		PoolAllocator allocator;
		allocator.initialize(mem, mem_size, object_size);

		// Allocate for every object
		for (unsigned int i = 0; i < object_count; i++)
		{
			allocator.allocate();
		}

		allocator.reset();

		// Try allocate every object again, and check return
		void* p;
		for (unsigned int i = 0; i < object_count; i++)
		{
			p = allocator.allocate();
			EXPECT_NE(p, nullptr);
		}

		free(mem);
	}
}

namespace TestComponentPool
{
	// Component used for testing
	struct TestComponent : public ecs::ECSComponent<TestComponent> { int data = -1; };

	// Memory information
	const unsigned int component_size = sizeof(TestComponent);
	const unsigned int component_count = 10;
	const unsigned int mem_size = component_size * component_count;

	TEST(TestComponentPool, Init)
	{
		// Create component information
		ecs::ECSComponentPool pool(component_count, component_size);

		ASSERT_EQ(pool.getAllocations(), 0);
		ASSERT_EQ(pool.getCurrentRemoveFlagCount(), 0);
	}
	TEST(TestComponentPool, Create)
	{
		ecs::ECSComponentPool pool(component_count, component_size);

		// Make a component in order to create components
		TestComponent init_data;

		// Try creating components
		// Set component data to i, and expect created component's data to be the same
		TestComponent* pComp;
		for (int i = 0; i < component_count; i++)
		{
			init_data.data = i;
			pComp = static_cast<TestComponent*>(pool.create(init_data));

			ASSERT_EQ(pComp->data, i);
		}
	}
	TEST(TestComponentPool, GetIterator)
	{
		ecs::ECSComponentPool pool(component_count, component_size);

		// Make a component in order to create components
		TestComponent init_data;

		// Create 10 components,
		// first comp. data is 0, last is 9 (used in validation later)
		for (int i = 0; i < component_count; i++)
		{
			init_data.data = i;
			pool.create(init_data);
		}

		ecs::ComponentIterator it = pool.getIterator();

		// Check if iterator is correct
		int comp_counter = 0;
		ecs::BaseComponent *pBase;
		TestComponent *p;
		while (pBase = it.next())
		{
			// Use counter to expect data
			p = static_cast<TestComponent*>(pBase);

			ASSERT_EQ(p->data, comp_counter);
			comp_counter++;
		}

		// Check how many components the iterator iterated, should be all
		ASSERT_EQ(comp_counter, component_count);
	}
	//TEST(TestComponentPool, IteratorAfterRemoval)
	//{

	//}
}