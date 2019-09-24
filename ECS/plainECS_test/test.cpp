#include "pch.h"

#include "ecs.h"
#include "ecsComponentIncludes.h"
#include "ecsSystemIncludes.h"
#include "ecsEventIncludes.h"

#include <unordered_map>
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

		ecs::BaseComponent* pBase = pool.getComponent(tester_ID);
		ASSERT_NE(pBase, nullptr);

		TestComponent* pComp = static_cast<TestComponent*>(pBase);
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
		ecs::BaseComponent* pBase;
		TestComponent* p;
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

		TestComponent* pComp;
		ecs::BaseComponent* pBase;
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
		for (int i = component_count - 1; i >= 0; i--)
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

		ecs::BaseEvent* pBase = pool.create(TestEvent(expected_data));
		EXPECT_NE(pBase, nullptr);

		TestEvent* pEvent = static_cast<TestEvent*>(pBase);
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

		TestEvent* pEvent;
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
		TestEvent* pEvent;
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
		ecs::Entity* pEntity;
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
		ecs::Entity* pEntity;
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
		ecs::Entity* pEntity;
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
		ecs::Entity* pEntity;
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

	const unsigned int component_count = 10;

	TEST(TestComponentManager, Init)
	{
		ecs::ECSComponentManager mgr;

		// Check default pre-initialization
		EXPECT_EQ(0, mgr.getTotalComponentCount());
		EXPECT_EQ(0, mgr.getComponentTypeCount());
		EXPECT_EQ(0, mgr.getComponentCountOfType(0)); // Check error TypeID
		EXPECT_EQ(0, mgr.getComponentCountOfType(TestComponent1::typeID)); // Check testing TypeID
		EXPECT_EQ(0, mgr.getInitializedComponentTypes().getRequirements().size());

		mgr.initPoolType(TestComponent1::typeID, TestComponent1::size, component_count);

		EXPECT_EQ(0, mgr.getTotalComponentCount());
		EXPECT_EQ(1, mgr.getComponentTypeCount());
		EXPECT_EQ(0, mgr.getComponentCountOfType(TestComponent1::typeID)); // Check testing TypeID
		EXPECT_EQ(1, mgr.getInitializedComponentTypes().getRequirements().size());
	}
	TEST(TestComponentManager, CreatePreInitiated)
	{
		ecs::ECSComponentManager mgr;

		// Initialize pool for testing component type before creating a component
		mgr.initPoolType(TestComponent1::typeID, TestComponent1::size, component_count);
		EXPECT_EQ(1, mgr.getInitializedComponentTypes().getRequirements().size()); // Quick check

		// Create components, check return pointer and data
		TestComponent1* pComponent;
		ecs::BaseComponent* pBase;
		for (int i = 0; i < component_count; i++)
		{
			// Set TestComponent1::data = i
			pBase = mgr.createComponent(TestComponent1(i));
			ASSERT_NE(nullptr, pBase);									// Component is created
			EXPECT_NE(0, pBase->getID());								// Has ID (0=Error)
			EXPECT_EQ(TestComponent1::typeID, pBase->getTypeID());		// Has correct TypeID

			pComponent = static_cast<TestComponent1*>(pBase);
			EXPECT_EQ(i, pComponent->data);								// Has correct data
		}

		EXPECT_EQ(component_count, mgr.getTotalComponentCount());
		EXPECT_EQ(1, mgr.getComponentTypeCount());
		EXPECT_EQ(component_count, mgr.getComponentCountOfType(TestComponent1::typeID)); // Check testing TypeID
		EXPECT_EQ(1, mgr.getInitializedComponentTypes().getRequirements().size());
	}
	TEST(TestComponentManager, CreateNotInitiated)
	{
		ecs::ECSComponentManager mgr;

		// Create components, check return pointer and data
		TestComponent1* pComponent;
		ecs::BaseComponent* pBase;
		for (int i = 0; i < component_count; i++)
		{
			// Set TestComponent1::data = i
			pBase = mgr.createComponent(TestComponent1(i));
			ASSERT_NE(nullptr, pBase);									// Component is created
			EXPECT_NE(0, pBase->getID());								// Has ID (0=Error)
			EXPECT_EQ(TestComponent1::typeID, pBase->getTypeID());		// Has correct TypeID

			pComponent = static_cast<TestComponent1*>(pBase);
			EXPECT_EQ(i, pComponent->data);								// Has correct data
		}

		EXPECT_EQ(component_count, mgr.getTotalComponentCount());
		EXPECT_EQ(1, mgr.getComponentTypeCount());
		EXPECT_EQ(component_count, mgr.getComponentCountOfType(TestComponent1::typeID)); // Check testing TypeID
		EXPECT_EQ(1, mgr.getInitializedComponentTypes().getRequirements().size());
	}
	TEST(TestComponentManager, CountGetters)
	{
		ecs::ECSComponentManager mgr;

		// Create components
		for (int i = 0; i < component_count; i++)
		{
			mgr.createComponent(TestComponent1(i));
			mgr.createComponent(TestComponent2(i));
		}

		EXPECT_EQ(mgr.getTotalComponentCount(), component_count * 2);
		EXPECT_EQ(mgr.getComponentTypeCount(), 2);
		EXPECT_EQ(mgr.getInitializedComponentTypes().getRequirements().size(), 2);
	}
	TEST(TestComponentManager, Get)
	{
		ecs::ECSComponentManager mgr;

		// Create components, store IDs
		ID id_list[component_count];
		for (int i = 0; i < component_count; i++)
		{
			id_list[i] = mgr.createComponent(TestComponent1(i))->getID();
		}

		// Try to retrive components
		TestComponent1* pComponent;
		ecs::BaseComponent* pBase;
		for (int i = 0; i < component_count; i++)
		{
			pBase = mgr.getComponent(TestComponent1::typeID, id_list[i]);
			ASSERT_NE(pBase, nullptr);

			pComponent = static_cast<TestComponent1*>(pBase);
			EXPECT_EQ(pComponent->data, i);
		}
	}
	TEST(TestComponentManager, FlagForRemoval)
	{
		ecs::ECSComponentManager mgr;

		// Create components, store IDs
		ID id_list[component_count];
		for (int i = 0; i < component_count; i++)
		{
			id_list[i] = mgr.createComponent(TestComponent1(i))->getID();
		}

		// Flag half for removal, count flags
		int expected_flags = 0;
		for (int i = 0; i < component_count; i += 2)
		{
			mgr.flagRemoval(TestComponent1::typeID, id_list[i]);
			expected_flags++;
		}

		EXPECT_EQ(expected_flags, mgr.getCurrentRemoveFlagCount());
	}
	TEST(TestComponentManager, Removal)
	{
		ecs::ECSComponentManager mgr;

		// Create components, store IDs
		ID id_list[component_count];
		for (int i = 0; i < component_count; i++)
		{
			id_list[i] = mgr.createComponent(TestComponent1(i))->getID();
		}

		// Flag half for removal, subtract from expected components per flag
		// Set removed IDs to 0, used later
		int expected_components = component_count;
		for (int i = 0; i < component_count; i += 2)
		{
			mgr.flagRemoval(TestComponent1::typeID, id_list[i]);
			id_list[i] = 0;
			expected_components--;
		}
		mgr.removeAllFlagged();

		EXPECT_EQ(mgr.getTotalComponentCount(), expected_components);

		// data = i
		TestComponent1* pComponent;
		ecs::BaseComponent* pBase;
		for (int i = 0; i < component_count; i++)
		{
			if (id_list[i] != 0)
			{
				pBase = mgr.getComponent(TestComponent1::typeID, id_list[i]);
				ASSERT_NE(pBase, nullptr);

				pComponent = static_cast<TestComponent1*>(pBase);
				EXPECT_EQ(pComponent->data, i);
			}
		}

		// Try get removed IDs
		for (int i = 0; i < component_count; i++)
		{
			if (id_list[i] == 0)
			{
				pBase = mgr.getComponent(TestComponent1::typeID, id_list[i]);
				ASSERT_EQ(pBase, nullptr);
			}
		}
	}
	TEST(TestComponentManager, Iterator)
	{
		ecs::ECSComponentManager mgr;

		// Create components, store IDs
		ID id;
		std::unordered_map<ID, int> expected_data;
		for (int i = 0; i < component_count; i++)
		{
			id = mgr.createComponent(TestComponent1(i))->getID();
			expected_data[id] = i;
		}

		ecs::ComponentIterator it = mgr.getComponentIterator(TestComponent1::typeID);

		// Check all components the iterator has, check their data and count
		int iterator_count = 0;
		TestComponent1* pComponent;
		ecs::BaseComponent* pBase;
		while (pBase = it.next())
		{
			pComponent = static_cast<TestComponent1*>(pBase);

			id = pComponent->getID();
			EXPECT_EQ(pComponent->data, expected_data[id]);
			iterator_count++;
		}
		EXPECT_EQ(iterator_count, component_count);
	}
	TEST(TestComponentManager, CreateAfterRemoval)
	{
		ecs::ECSComponentManager mgr;

		// Create components, store IDs
		ID id_list[component_count];
		for (int i = 0; i < component_count; i++)
		{
			id_list[i] = mgr.createComponent(TestComponent1(i))->getID();
		}

		// Remove all components, back to front
		for (int i = component_count - 1; i >= 0; i--)
		{
			mgr.removeComponent(TestComponent1::typeID, id_list[i]);
		}

		// Create more components, then check if they have the correct data
		TestComponent1* pComponent;
		ecs::BaseComponent* pBase;
		for (int i = 0; i < component_count; i++)
		{
			pBase = mgr.createComponent(TestComponent1(i));
			EXPECT_NE(pBase, nullptr);

			pComponent = static_cast<TestComponent1*>(pBase);
			EXPECT_EQ(pComponent->data, i);
		}
	}
	TEST(TestComponentManager, IteratorAfterSomeRemovals)
	{
		ecs::ECSComponentManager mgr;

		// Create components, store IDs
		ID id_list[component_count];
		std::map<ID, int> expected_data;
		for (int i = 0; i < component_count; i++)
		{
			id_list[i] = mgr.createComponent(TestComponent1(i))->getID();
		}

		// Remove half of the components, keep count how many components
		// are still existing. Set removed component IDs to 0.
		int expected_component_count = component_count;
		for (int i = 0; i < component_count; i++)
		{
			mgr.removeComponent(TestComponent1::typeID, id_list[i]);
			id_list[i] = 0;
			expected_component_count--;
		}

		ecs::ComponentIterator it = mgr.getComponentIterator(TestComponent1::typeID);

		// Check component in iterator, keep count of components in iterator
		int iterator_component_count = 0;
		TestComponent1* pComponent;
		ecs::BaseComponent* pBase;
		while (pBase = it.next())
		{
			ASSERT_NE(pBase, nullptr);
			pComponent = static_cast<TestComponent1*>(pBase);

			// Check if data is correct
			ID id = pComponent->getID();
			EXPECT_EQ(pComponent->data, expected_data[id]);
			iterator_component_count++;
		}
		EXPECT_EQ(iterator_component_count, expected_component_count);
	}
} // namespace TestComponentManager
namespace TestEventManager
{
	struct TestEvent : public ecs::ECSEvent<TestEvent>
	{
		TestEvent(int _data) : data(_data) {}
		int data;
	};

	class SubSystem : public ecs::ECSEventListener
	{
	public:
		SubSystem() {}
		virtual ~SubSystem() {}
		void onEvent(TypeID _eventType, ecs::BaseEvent* _event)
		{
			if (_eventType == TestEvent::typeID && _event && _event->getTypeID() == TestEvent::typeID)
			{
				gotEvent = true;
			}
		};
		bool gotEvent = false;
	};

	const unsigned int event_count = 10;

	TEST(TestEventManager, Init)
	{
		ecs::ECSEventManager mgr;

		// Expect default values
		EXPECT_EQ(mgr.getTotalEventCount(), 0);
		EXPECT_EQ(mgr.getEventIterator(0).size(), 0);
		EXPECT_EQ(mgr.getEventIterator(TestEvent::typeID).size(), 0);
	}
	TEST(TestEventManager, Create)
	{
		ecs::ECSEventManager mgr;

		// Try create events, check if they are initialized correctly
		TestEvent* pEvent;
		for (int i = 0; i < event_count; i++)
		{
			ecs::BaseEvent* pBase = mgr.createEvent(TestEvent(i));
			ASSERT_NE(pBase, nullptr);
			ASSERT_EQ(pBase->getTypeID(), TestEvent::typeID);

			pEvent = static_cast<TestEvent*>(pBase);
			EXPECT_EQ(pEvent->data, i);
		}
		EXPECT_EQ(mgr.getTotalEventCount(), event_count);
	}
	TEST(TestEventManager, Remove)
	{
		ecs::ECSEventManager mgr;

		// Try create events
		TestEvent* pEvent;
		for (int i = 0; i < event_count; i++)
		{
			mgr.createEvent(TestEvent(i));
		}

		mgr.clearAllEvents();
		EXPECT_EQ(mgr.getTotalEventCount(), 0);
	}
	TEST(TestEventManager, GetCount)
	{
		ecs::ECSEventManager mgr;

		// Try create events
		for (int i = 0; i < event_count; i++)
		{
			mgr.createEvent(TestEvent(i));
		}
		EXPECT_EQ(mgr.getTotalEventCount(), event_count);
	}
	TEST(TestEventManager, Subscriber)
	{
		ecs::ECSEventManager mgr;
		SubSystem subSys;
		subSys.gotEvent = false;

		mgr.addEventSubscriber(TestEvent::typeID, &subSys);
		mgr.createEvent(TestEvent(0));
		EXPECT_TRUE(subSys.gotEvent);

		subSys.gotEvent = false;
		mgr.clearAllEvents();
		mgr.removeEventSubscriber(TestEvent::typeID, &subSys);

		mgr.createEvent(TestEvent(0));
		EXPECT_FALSE(subSys.gotEvent);
	}
	TEST(TestEventManager, GetIterator)
	{
		ecs::ECSEventManager mgr;

		// Try create events
		for (int i = 0; i < event_count; i++)
		{
			mgr.createEvent(TestEvent(i));
		}

		std::vector<ecs::BaseEvent*> it = mgr.getEventIterator(TestEvent::typeID);
		EXPECT_EQ(it.size(), event_count);

		int expected_data = 0;
		TestEvent* pEvent;
		for (ecs::BaseEvent* pBase : it)
		{
			pEvent = static_cast<TestEvent*>(pBase);
			EXPECT_EQ(pEvent->data, expected_data++);
		}
	}
	TEST(TestEventManager, CreateAfterRemove)
	{
		ecs::ECSEventManager mgr;

		for (int i = 0; i < event_count; i++)
		{
			mgr.createEvent(TestEvent(i));
		}

		mgr.clearAllEvents();

		// Try create more events
		ecs::BaseEvent* pBase;
		for (int i = 0; i < event_count; i++)
		{
			pBase = mgr.createEvent(TestEvent(i));
			EXPECT_NE(pBase, nullptr);
		}
	}
	TEST(TestEventManager, IteratorAfterRemove)
	{
		ecs::ECSEventManager mgr;

		for (int i = 0; i < event_count; i++)
		{
			mgr.createEvent(TestEvent(i));
		}

		mgr.clearAllEvents();

		std::vector<ecs::BaseEvent*> it = mgr.getEventIterator(TestEvent::typeID);
		EXPECT_EQ(it.size(), 0);
	}
} // namespace TestEventManager
#pragma endregion ManagerTesting
#pragma region ECSUserTesting	// NOT DONE
namespace TestECSUserCore
{
	//TEST(TestECSUser, Init)
} // TestECSUserCore
namespace TestECSUserEntity
{
	//TEST(TestECSUser, CreateEntity)
	//TEST(TestECSUser, RemoveEntity)
	//TEST(TestECSUser, GetEntity)
} // TestECSUserEntity
namespace TestECSUserComponent
{
	//TEST(TestECSUser, )
	//TEST(TestECSUser, )
	//TEST(TestECSUser, )
} // TestECSUserComponent
namespace TestECSUserSystem
{
	//TEST(TestECSUser, )
	//TEST(TestECSUser, )
	//TEST(TestECSUser, )
} // TestECSUserSystem
namespace TestECSUserComponent
{
	//TEST(TestECSUser, )
	//TEST(TestECSUser, )
	//TEST(TestECSUser, )
} // TestECSUserComponent
#pragma endregion ECSUserTesting
#pragma region ECSTesting // NOT DONE
namespace TestECS
{
	/*
		This counter is used to check in which order systems are
		updated. In each system's update, the system will fetch
		the current value in g_update_ticket_counter and increase
		it by one.
	*/
	int g_update_ticket_counter = 0;

	#pragma region TestDataTypes
	struct TestComponent : public ecs::ECSComponent<TestComponent>
	{
		TestComponent(int _data) : data(_data) {}
		int data;
	};

	struct TestEvent : public ecs::ECSEvent<TestEvent>
	{
		TestEvent(int _data) : data(_data) {}
		int data;
	};

	class TestEntityUpdateSystem : public ecs::ECSSystem<TestEntityUpdateSystem>
	{
	public:
		bool isUpdated = false;
		int updateTicket = 0;
		TestEntityUpdateSystem()
		{
			updateType = ecs::SystemUpdateType::EntityUpdate;
			typeFilter.addRequirement(TestComponent::typeID);
		}
		virtual ~TestEntityUpdateSystem() {}
		void updateEntity(ecs::FilteredEntity& _entityInfo, float _delta) override
		{
			isUpdated = true;
			updateTicket = g_update_ticket_counter++;
		}
	};
	class TestMultipleEntityUpdateSystem: public ecs::ECSSystem<TestMultipleEntityUpdateSystem>
	{
	public:
		bool isUpdated = false;
		int updateTicket = 0;
		TestMultipleEntityUpdateSystem()
		{
			updateType = ecs::SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(TestComponent::typeID);
		}
		virtual ~TestMultipleEntityUpdateSystem() {}
		void updateMultipleEntities(ecs::EntityIterator& _entityInfo, float _delta) override
		{
			isUpdated = true;
			updateTicket = g_update_ticket_counter++;
		}
	};
	class TestEventReaderSystem : public ecs::ECSSystem<TestEventReaderSystem>
	{
	public:
		bool isUpdated = false;
		int updateTicket = 0;
		TestEventReaderSystem()
		{
			updateType = ecs::SystemUpdateType::EventReader;
			typeFilter.addRequirement(TestEvent::typeID);
		}
		virtual ~TestEventReaderSystem() {}
		void readEvent(ecs::BaseEvent& _event, float _delta) override
		{
			isUpdated = true;
			updateTicket = g_update_ticket_counter++;
		}
	};
	class TestEventListenerSystem : public ecs::ECSSystem<TestEventListenerSystem>
	{
	public:
		bool isUpdated = false;
		int updateTicket = 0;
		TestEventListenerSystem()
		{
			updateType = ecs::SystemUpdateType::EventListenerOnly;
			subscribeEventCreation(TestEvent::typeID);
		}
		virtual ~TestEventListenerSystem() {}
		void onEvent(TypeID _eventType, ecs::BaseEvent* _event) override
		{
			isUpdated = true;
			updateTicket = g_update_ticket_counter++;
		}
	};
	class TestActorSystem : public ecs::ECSSystem<TestActorSystem>
	{
	public:
		bool isUpdated = false;
		int updateTicket = 0;
		TestActorSystem()
		{
			updateType = ecs::SystemUpdateType::Actor;
		}
		virtual ~TestActorSystem() {}
		void act(float _delta) override
		{
			isUpdated = true;
			updateTicket = g_update_ticket_counter++;
		}
	};
	#pragma endregion TestDataTypes

	namespace TestECSCore
	{
		TEST(TestECSCore, Init)
		{
			ecs::EntityComponentSystem ecs;

			EXPECT_EQ(ecs.getTotalEntityCount(), 0);
			EXPECT_EQ(ecs.getTotalEntityCount(), 0);
			EXPECT_EQ(ecs.getTotalSystemCount(), 0);
		}
		TEST(TestECSCore, Update)
		{
			ecs::EntityComponentSystem ecs;

			// Create systems
			TestEventListenerSystem			*p_elSystem  = ecs.createSystem<TestEventListenerSystem>();
			TestEntityUpdateSystem			*p_euSystem	 = ecs.createSystem<TestEntityUpdateSystem>();
			TestMultipleEntityUpdateSystem	*p_meuSystem = ecs.createSystem<TestMultipleEntityUpdateSystem>();
			TestEventReaderSystem			*p_erSystem  = ecs.createSystem<TestEventReaderSystem>();
			TestActorSystem					*p_aSystem	 = ecs.createSystem<TestActorSystem>();

			ecs.createEntity(TestComponent(1337));
			ecs.createEvent(TestEvent(1337));

			// p_elSystem subscribes on TestEvent creation. Let's check that first.
			EXPECT_TRUE(p_elSystem->isUpdated);

			// Test all other system types

			// Create a map for expected update ticket number (order of execution)
			std::unordered_map<TypeID, int> expected_tickets;
			expected_tickets[TestEntityUpdateSystem::typeID]			= 1;
			expected_tickets[TestMultipleEntityUpdateSystem::typeID]	= 2;
			expected_tickets[TestEventReaderSystem::typeID]				= 3;
			expected_tickets[TestActorSystem::typeID]					= 4;

			// Set first ticket to be 1
			g_update_ticket_counter = 1;

			EXPECT_FALSE(p_euSystem->isUpdated);
			EXPECT_FALSE(p_meuSystem->isUpdated);
			EXPECT_FALSE(p_erSystem->isUpdated);
			EXPECT_FALSE(p_aSystem->isUpdated);

			ecs.update(0.0f);

			EXPECT_TRUE(p_euSystem->isUpdated);
			EXPECT_TRUE(p_meuSystem->isUpdated);
			EXPECT_TRUE(p_erSystem->isUpdated);
			EXPECT_TRUE(p_aSystem->isUpdated);

			EXPECT_EQ(p_euSystem->updateTicket, expected_tickets[p_euSystem->getTypeID()]);
			EXPECT_EQ(p_meuSystem->updateTicket, expected_tickets[p_meuSystem->getTypeID()]);
			EXPECT_EQ(p_erSystem->updateTicket, expected_tickets[p_erSystem->getTypeID()]);
			EXPECT_EQ(p_aSystem->updateTicket, expected_tickets[p_aSystem->getTypeID()]);
		}
	} // TestECSCore
	namespace TestECSEntity
	{
		TEST(TestECSEntity, CreateEntity)
		{
			ecs::EntityComponentSystem ecs;

			ecs::Entity *p_entity = ecs.createEntity(TestComponent(1));
			ASSERT_NE(p_entity, nullptr);

			// Check ECS data
			EXPECT_EQ(ecs.getTotalEntityCount(), 1);

			// Check entity data
			// ID that is 0 is an error flag
			EXPECT_NE(p_entity->getID(), 0); 
			EXPECT_TRUE(p_entity->hasComponentOfType(TestComponent::typeID));
			EXPECT_TRUE(p_entity->hasComponentOfType<TestComponent>());
			EXPECT_NE(p_entity->getComponentID(TestComponent::typeID), 0);
			EXPECT_NE(p_entity->getComponentID<TestComponent>(), 0);
			EXPECT_EQ(p_entity->getComponentCount(), 1);
		}
		TEST(TestECSEntity, RemoveEntity)
		{
			ecs::EntityComponentSystem ecs;
			ecs::Entity *p_entity = ecs.createEntity(TestComponent(1));

			ecs.removeEntity(p_entity->getID()); // Flags removal
			ecs.update(0.f); // Actual removals happen end of update

			EXPECT_EQ(ecs.getTotalEntityCount(), 0);
		}
		TEST(TestECSEntity, GetEntity)
		{
			ecs::EntityComponentSystem ecs;
			ID entity_id = ecs.createEntity(TestComponent(1))->getID();

			ecs::Entity *p_entity = ecs.getEntity(entity_id);
			EXPECT_NE(p_entity, nullptr);

			ecs::TypeFilter filter;
			filter.addRequirement(TestComponent::typeID);
			ecs::EntityIterator it = ecs.getEntititesByFilter(filter);
			ASSERT_EQ(it.entities.size(), 1);
			EXPECT_EQ(it.entities.front().entity->getID(), entity_id);
		}
	} // TestECSEntity

	namespace TestECSComponent
	{
		//TEST(TestECSComponent, CreateComponent)
		//TEST(TestECSComponent, RemoveComponent)
		//TEST(TestECSComponent, GetComponent)
	} // TestECSComponent
	namespace TestECSEvent
	{
		//TEST(TestECSEvent, CreateEvent)
		//TEST(TestECSEvent, RemoveEvent)
		//TEST(TestECSEvent, GetEvent)
	} // TestECSEvent
	namespace TestECSSystem
	{
		//TEST(TestECSSystem, CreateSystem)
		//TEST(TestECSSystem, RemoveSystem)
	} // TestECSSystem
}
#pragma endregion ECSTesting