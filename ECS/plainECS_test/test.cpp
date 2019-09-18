#include "pch.h"

#include <ecs.h>
#pragma comment(lib, "plainECS.lib")

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

#pragma region MemoryTesting
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
} // namespace TestPoolAllocator
namespace TestComponentPool
{
	// Component used for testing
	struct TestComponent : public ecs::ECSComponent<TestComponent>
	{
		TestComponent(int initData = -1) : data(initData) {};
		int data;
	};

	// Memory information
	const unsigned int component_size = sizeof(TestComponent);
	const unsigned int component_count = 10;
	const unsigned int mem_size = component_size * component_count;

	TEST(TestComponentPool, Init)
	{
		// Create component information
		ecs::ECSComponentPool pool;
		
		ASSERT_FALSE(pool.isInitialized());
		pool.initialize(component_count, component_size);
		ASSERT_TRUE(pool.isInitialized());
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
		unsigned int allocation_count = 0;
		for (int i = 0; i < component_count; i++)
		{
			pComp = static_cast<TestComponent*>(pool.create(TestComponent(i)));
			EXPECT_EQ(pComp->data, i);
			
			allocation_count++;
		}

		// Check if allocation count is correct
		EXPECT_EQ(pool.getAllocations(), allocation_count);
	}
	TEST(TestComponentPool, Get)
	{
		ecs::ECSComponentPool pool(component_count, component_size);

		// Create components, store id and data for one of them
		ID tester_ID;
		int tester_data;
		for (int i = 0; i < component_count; i++)
		{
			ID id = pool.create(TestComponent(i))->getID();

			if (i == (component_count / 2))
			{
				tester_ID = id;
				tester_data = i;
			}
		}

		ecs::BaseComponent *pBase = pool.getComponent(tester_ID);
		ASSERT_NE(pBase, nullptr);

		TestComponent *pComp = static_cast<TestComponent*>(pBase);
		EXPECT_EQ(pComp->data, tester_data);
	}
	TEST(TestComponentPool, GetIterator)
	{
		ecs::ECSComponentPool pool(component_count, component_size);

		// Create 10 components,
		// first comp. data is 0, last is 9 (used in validation later)
		for (int i = 0; i < component_count; i++)
		{
			pool.create(TestComponent(i));
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
	TEST(TestComponentPool, Removal)
	{
		ecs::ECSComponentPool pool(component_count, component_size);

		// Create 10 components with data in range [0,9] 
		// Store IDs
		ID idList[component_count];
		for (int i = 0; i < component_count; i++)
		{
			idList[i] = (pool.create(TestComponent(i)))->getID();
		}

		// Remove half of the components, starting from index 1
		// No component with an odd number in data should exist.
		// Store how many components there are left, used later.
		int current_count = component_count;
		for (int i = 1; i < component_count; i += 2)
		{
			pool.remove(idList[i]);
			current_count--;
		}

		// Check if count in pool is correct
		EXPECT_EQ(current_count, pool.getAllocations());

		ecs::ComponentIterator it = pool.getIterator();

		TestComponent *pComp;
		ecs::BaseComponent *pBase;
		while (pBase = it.next())
		{
			pComp = static_cast<TestComponent*>(pBase);

			// Expect even number
			EXPECT_EQ(pComp->data % 2, 0);

			// Decrease components to check.
			current_count--;
		}
		
		// current_count should be zero after iteration,
		// since it iterated all existing components
		EXPECT_EQ(current_count, 0);
	}
	TEST(TestComponentPool, CreateAfterRemoval)
	{
		ecs::ECSComponentPool pool(component_count, component_size);

		// Create 10 components with data in range [0,9] 
		// Store IDs
		ID idList[component_count];
		for (int i = 0; i < component_count; i++)
		{
			idList[i] = (pool.create(TestComponent(i)))->getID();
		}

		// Remove all
		for (int i = component_count-1; i >= 0; i--)
		{
			pool.remove(idList[i]);
		}

		// Create 10 new components
		for (int i = 0; i < component_count; i++)
		{
			EXPECT_NE(pool.create(TestComponent(i)), nullptr);
		}
	}
	TEST(TestComponentPool, CurrentFlagCount)
	{
		ecs::ECSComponentPool pool(component_count, component_size);

		// Create 10 components with data in range [0,9] 
		// Store IDs
		ID idList[component_count];
		for (int i = 0; i < component_count; i++)
		{
			idList[i] = (pool.create(TestComponent(i)))->getID();
		}

		// Flag half, keep count of how many flags been made
		int flag_count = 0;
		for (int i = 0; i < component_count; i += 2)
		{
			pool.flagRemoval(idList[i]);
			flag_count++;
		}

		// Check flag count in pool
		EXPECT_EQ(pool.getCurrentRemoveFlagCount(), flag_count);
	}
	TEST(TestComponentPool, HandleOutOfRange)
	{
		const int invalid_ID = -1;
		ecs::ECSComponentPool pool(component_count, component_size);

		// Create 10 components with data in range [0,9] 
		for (int i = 0; i < component_count; i++)
		{
			pool.create(TestComponent(i));
		}

		EXPECT_EQ(pool.getComponent(invalid_ID), nullptr);
	}
} // namespace TestComponentPool
namespace TestEventPool
{
	struct TestEvent : public ecs::ECSEvent<TestEvent>
	{
		TestEvent(int _data) : data(_data) {};
		int data;
	};

	const unsigned int event_count = 10;

	TEST(TestEventPool, Init)
	{
		ecs::ECSEventPool pool;
		EXPECT_EQ(pool.getEventCount(), 0);
		
		// Expect empty iterator
		std::vector<ecs::BaseEvent*> it = pool.getIterator();
		EXPECT_EQ(it.size(), 0);
	}
	TEST(TestEventPool, Create)
	{
		int expected_data = 1337;
		ecs::ECSEventPool pool;

		ecs::BaseEvent *pBase = pool.create(TestEvent(expected_data));
		EXPECT_NE(pBase, nullptr);

		TestEvent *pEvent = static_cast<TestEvent*>(pBase);
		EXPECT_EQ(pEvent->data, expected_data);
	}
	TEST(TestEventPool, Iterator)
	{
		ecs::ECSEventPool pool;

		// Create events, store data in order
		int expected_data[event_count];
		for (int i = 0; i < event_count; i++)
		{
			pool.create(TestEvent(i));
			expected_data[i] = i;
		}

		std::vector<ecs::BaseEvent*> it = pool.getIterator();

		TestEvent *pEvent;
		for (int i = 0; i < event_count; i++)
		{
			pEvent = static_cast<TestEvent*>(it[i]);
			EXPECT_EQ(pEvent->data, expected_data[i]);
		}
	}
	TEST(TestEventPool, Clear)
	{
		ecs::ECSEventPool pool;

		// Create events
		for (int i = 0; i < event_count; i++)
		{
			pool.create(TestEvent(i));
		}

		pool.clear();

		EXPECT_EQ(pool.getEventCount(), 0);

		// Expect empty iterator
		EXPECT_EQ(pool.getIterator().size(), 0);
	}
	TEST(TestEventPool, GetCount)
	{
		ecs::ECSEventPool pool;

		// Create events
		for (int i = 0; i < event_count; i++)
		{
			pool.create(TestEvent(i));
		}

		EXPECT_EQ(pool.getEventCount(), event_count);
	}
	TEST(TestEventPool, CreateAfterClear)
	{
		ecs::ECSEventPool pool;

		// Create events
		for (int i = 0; i < event_count; i++)
		{
			pool.create(TestEvent(0));
		}

		pool.clear();

		// Create more events after clear, store data
		int expected_data[event_count];
		for (int i = 0; i < event_count; i++)
		{
			pool.create(TestEvent(i));
			expected_data[i] = i;
		}

		std::vector<ecs::BaseEvent*> it = pool.getIterator();

		// Check data of new events
		TestEvent *pEvent;
		for (int i = 0; i < event_count; i++)
		{
			pEvent = static_cast<TestEvent*>(it[i]);
			EXPECT_EQ(pEvent->data, expected_data[i]);
		}
	}
} // namespace TestEventPool
#pragma endregion MemoryTesting
#pragma region ManagerTesting
namespace TestEntityManager
{
	const unsigned int entity_count = 10;

	TEST(TestEntityManager, Init)
	{
		ecs::ECSEntityManager mgr;

		EXPECT_EQ(0, mgr.getCurrentRemoveFlagCount());
		EXPECT_EQ(0, mgr.getEntityCount());
	}
	TEST(TestEntityManager, Create)
	{
		ecs::ECSEntityManager mgr;

		// Create entities, check their default values
		ecs::Entity *pEntity;
		for (int i = 0; i < entity_count; i++)
		{
			pEntity = mgr.createEntity();
			ASSERT_NE(nullptr, pEntity);
			EXPECT_NE(0, pEntity->getID());
			EXPECT_EQ(0, pEntity->getComponentCount());
		}
	}
	TEST(TestEntityManager, GetEntity)
	{
		ecs::ECSEntityManager mgr;

		// Create entities, store their IDs
		ID id_list[entity_count];
		for (int i = 0; i < entity_count; i++)
		{
			id_list[i] = (mgr.createEntity())->getID();
		}

		// Retrieve entities, check return pointer and ID
		ecs::Entity *pEntity;
		for (int i = 0; i < entity_count; i++)
		{
			pEntity = mgr.getEntity(id_list[i]);
			EXPECT_NE(nullptr, pEntity);
			EXPECT_EQ(id_list[i], pEntity->getID());
		}
	}
	TEST(TestEntityManager, GetCount)
	{
		ecs::ECSEntityManager mgr;

		// Create entities
		for (int i = 0; i < entity_count; i++)
		{
			mgr.createEntity();
		}
		EXPECT_EQ(entity_count, mgr.getEntityCount());
	}
	TEST(TestEntityManager, Removal)
	{
		ecs::ECSEntityManager mgr;

		// Create entities, store their IDs, keep count
		int expected_count = 0;
		ID id_list[entity_count];
		for (int i = 0; i < entity_count; i++)
		{
			id_list[i] = (mgr.createEntity())->getID();
			expected_count++;
		}

		// Flag half for removal
		// Set removed entry in id_list to 0, used later
		for (int i = 0; i < entity_count; i += 2)
		{
			mgr.flagRemoval(id_list[i]);
			id_list[i] = 0;
			expected_count--;
		}

		int expected_flags = entity_count - expected_count;
		EXPECT_EQ(expected_flags, mgr.getCurrentRemoveFlagCount());

		mgr.removeAllFlagged();
		EXPECT_EQ(expected_count, mgr.getEntityCount());

		// Check that existing entites has the correct IDs
		ecs::Entity *pEntity;
		for (int i = 0; i < entity_count; i++)
		{
			// Only check entity IDs that hasn't been removed
			if (id_list[i] != 0)
			{
				pEntity = mgr.getEntity(id_list[i]);
				EXPECT_EQ(id_list[i], pEntity->getID());
			}
		}
	}
	TEST(TestEntityManager, CreateAfterRemove)
	{
		ecs::ECSEntityManager mgr;

		// Create entities, store their IDs
		ID id_list[entity_count];
		for (int i = 0; i < entity_count; i++)
		{
			id_list[i] = (mgr.createEntity())->getID();
		}

		// Remove all entities, in other order
		for (int i = entity_count - 1; i >= 0; i--)
		{
			mgr.flagRemoval(id_list[i]);
		}
		mgr.removeAllFlagged();

		// Create new entities, check IDs
		ecs::Entity *pEntity;
		for (int i = 0; i < entity_count; i++)
		{
			pEntity = mgr.createEntity();
			EXPECT_NE(nullptr, pEntity);
			EXPECT_NE(0, pEntity->getID());
		}
	}
} // namespace TestEntityManager
namespace TestComponentManager
{
	struct TestComponent1 : public ecs::ECSComponent<TestComponent1>
	{
		TestComponent1(int _data) : data(_data) {}
		int data;
	};

	struct TestComponent2 : public ecs::ECSComponent<TestComponent2>
	{
		TestComponent2(int _data) : data(_data) {}
		int data;
	};

	//TEST(TestComponentManager, Init)
	//{
	//	ecs::ECSComponentManager mgr;

	//	EXPECT_EQ(0, mgr.getTotalComponentCount());
	//	EXPECT_EQ(0, mgr.getComponentTypeCount());
	//	EXPECT_EQ(0, mgr.getInitializedComponentTypes());
	//	//EXPECT_EQ(0, mgr.get());
	//}
	//TEST(TestComponentManager, CreatePreInitiated)
	//TEST(TestComponentManager, CreateNotInitiated)
	//TEST(TestComponentManager, CountGetters)
	//TEST(TestComponentManager, Get)
	//TEST(TestComponentManager, FlagForRemoval)
	//TEST(TestComponentManager, Removal)
	//TEST(TestComponentManager, Iterator)
} // namespace TestComponentManager
namespace TestEventManager
{

} // namespace TestEventManager
#pragma endregion ManagerTesting