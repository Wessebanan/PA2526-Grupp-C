#include "pch.h"

#include "ecs.h"
#include "ecsObjectsForTesting.h"

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
	// Memory information
	const unsigned int component_size = sizeof(ecs::components::TestComponent);
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
		ecs::components::TestComponent init_data;

		// Try creating components
		// Set component data to i, and expect created component's data to be the same
		ecs::components::TestComponent* pComp;
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
	const unsigned int component_count = 10;

	TEST(TestComponentManager, Init)
	{
		ecs::ECSComponentManager mgr;

		// Check default pre-initialization
		EXPECT_EQ(0, mgr.getTotalComponentCount());
		EXPECT_EQ(0, mgr.getComponentTypeCount());
		EXPECT_EQ(0, mgr.getComponentCountOfType(0)); // Check error TypeID
		EXPECT_EQ(0, mgr.getComponentCountOfType(ecs::components::TestComponent::typeID)); // Check testing TypeID
		EXPECT_EQ(0, mgr.getInitializedComponentTypes().getRequirements().size());

		mgr.initPoolType(ecs::components::TestComponent::typeID, ecs::components::TestComponent::size, component_count);

		EXPECT_EQ(0, mgr.getTotalComponentCount());
		EXPECT_EQ(1, mgr.getComponentTypeCount());
		EXPECT_EQ(0, mgr.getComponentCountOfType(ecs::components::TestComponent::typeID)); // Check testing TypeID
		EXPECT_EQ(1, mgr.getInitializedComponentTypes().getRequirements().size());
	}
	TEST(TestComponentManager, CreatePreInitiated)
	{
		ecs::ECSComponentManager mgr;

		// Initialize pool for testing component type before creating a component
		mgr.initPoolType(ecs::components::TestComponent::typeID, ecs::components::TestComponent::size, component_count);
		EXPECT_EQ(1, mgr.getInitializedComponentTypes().getRequirements().size()); // Quick check

		// Create components, check return pointer and data
		ecs::components::TestComponent* pComponent;
		ecs::BaseComponent* pBase;
		for (int i = 0; i < component_count; i++)
		{
			// Set ecs::components::TestComponent::data = i
			pBase = mgr.createComponent(ecs::components::TestComponent(i));
			ASSERT_NE(nullptr, pBase);									// Component is created
			EXPECT_NE(0, pBase->getID());								// Has ID (0=Error)
			EXPECT_EQ(ecs::components::TestComponent::typeID, pBase->getTypeID());		// Has correct TypeID

			pComponent = static_cast<ecs::components::TestComponent*>(pBase);
			EXPECT_EQ(i, pComponent->data);								// Has correct data
		}

		EXPECT_EQ(component_count, mgr.getTotalComponentCount());
		EXPECT_EQ(1, mgr.getComponentTypeCount());
		EXPECT_EQ(component_count, mgr.getComponentCountOfType(ecs::components::TestComponent::typeID)); // Check testing TypeID
		EXPECT_EQ(1, mgr.getInitializedComponentTypes().getRequirements().size());
	}
	TEST(TestComponentManager, CreateNotInitiated)
	{
		ecs::ECSComponentManager mgr;

		// Create components, check return pointer and data
		ecs::components::TestComponent* pComponent;
		ecs::BaseComponent* pBase;
		for (int i = 0; i < component_count; i++)
		{
			// Set ecs::components::TestComponent::data = i
			pBase = mgr.createComponent(ecs::components::TestComponent(i));
			ASSERT_NE(nullptr, pBase);									// Component is created
			EXPECT_NE(0, pBase->getID());								// Has ID (0=Error)
			EXPECT_EQ(ecs::components::TestComponent::typeID, pBase->getTypeID());		// Has correct TypeID

			pComponent = static_cast<ecs::components::TestComponent*>(pBase);
			EXPECT_EQ(i, pComponent->data);								// Has correct data
		}

		EXPECT_EQ(component_count, mgr.getTotalComponentCount());
		EXPECT_EQ(1, mgr.getComponentTypeCount());
		EXPECT_EQ(component_count, mgr.getComponentCountOfType(ecs::components::TestComponent::typeID)); // Check testing TypeID
		EXPECT_EQ(1, mgr.getInitializedComponentTypes().getRequirements().size());
	}
	TEST(TestComponentManager, CountGetters)
	{
		ecs::ECSComponentManager mgr;

		// Create components
		for (int i = 0; i < component_count; i++)
		{
			mgr.createComponent(ecs::components::TestComponent(i));
			mgr.createComponent(ecs::components::OtherTestComponent(i));
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
			id_list[i] = mgr.createComponent(ecs::components::TestComponent(i))->getID();
		}

		// Try to retrive components
		ecs::components::TestComponent* pComponent;
		ecs::BaseComponent* pBase;
		for (int i = 0; i < component_count; i++)
		{
			pBase = mgr.getComponent(ecs::components::TestComponent::typeID, id_list[i]);
			ASSERT_NE(pBase, nullptr);

			pComponent = static_cast<ecs::components::TestComponent*>(pBase);
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
			id_list[i] = mgr.createComponent(ecs::components::TestComponent(i))->getID();
		}

		// Flag half for removal, count flags
		int expected_flags = 0;
		for (int i = 0; i < component_count; i += 2)
		{
			mgr.flagRemoval(ecs::components::TestComponent::typeID, id_list[i]);
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
			id_list[i] = mgr.createComponent(ecs::components::TestComponent(i))->getID();
		}

		// Flag half for removal, subtract from expected components per flag
		// Set removed IDs to 0, used later
		int expected_components = component_count;
		for (int i = 0; i < component_count; i += 2)
		{
			mgr.flagRemoval(ecs::components::TestComponent::typeID, id_list[i]);
			id_list[i] = 0;
			expected_components--;
		}
		mgr.removeAllFlagged();

		EXPECT_EQ(mgr.getTotalComponentCount(), expected_components);

		// data = i
		ecs::components::TestComponent* pComponent;
		ecs::BaseComponent* pBase;
		for (int i = 0; i < component_count; i++)
		{
			if (id_list[i] != 0)
			{
				pBase = mgr.getComponent(ecs::components::TestComponent::typeID, id_list[i]);
				ASSERT_NE(pBase, nullptr);

				pComponent = static_cast<ecs::components::TestComponent*>(pBase);
				EXPECT_EQ(pComponent->data, i);
			}
		}

		// Try get removed IDs
		for (int i = 0; i < component_count; i++)
		{
			if (id_list[i] == 0)
			{
				pBase = mgr.getComponent(ecs::components::TestComponent::typeID, id_list[i]);
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
			id = mgr.createComponent(ecs::components::TestComponent(i))->getID();
			expected_data[id] = i;
		}

		ecs::ComponentIterator it = mgr.getComponentIterator(ecs::components::TestComponent::typeID);

		// Check all components the iterator has, check their data and count
		int iterator_count = 0;
		ecs::components::TestComponent* pComponent;
		ecs::BaseComponent* pBase;
		while (pBase = it.next())
		{
			pComponent = static_cast<ecs::components::TestComponent*>(pBase);

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
			id_list[i] = mgr.createComponent(ecs::components::TestComponent(i))->getID();
		}

		// Remove all components, back to front
		for (int i = component_count - 1; i >= 0; i--)
		{
			mgr.removeComponent(ecs::components::TestComponent::typeID, id_list[i]);
		}

		// Create more components, then check if they have the correct data
		ecs::components::TestComponent* pComponent;
		ecs::BaseComponent* pBase;
		for (int i = 0; i < component_count; i++)
		{
			pBase = mgr.createComponent(ecs::components::TestComponent(i));
			EXPECT_NE(pBase, nullptr);

			pComponent = static_cast<ecs::components::TestComponent*>(pBase);
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
			id_list[i] = mgr.createComponent(ecs::components::TestComponent(i))->getID();
		}

		// Remove half of the components, keep count how many components
		// are still existing. Set removed component IDs to 0.
		int expected_component_count = component_count;
		for (int i = 0; i < component_count; i++)
		{
			mgr.removeComponent(ecs::components::TestComponent::typeID, id_list[i]);
			id_list[i] = 0;
			expected_component_count--;
		}

		ecs::ComponentIterator it = mgr.getComponentIterator(ecs::components::TestComponent::typeID);

		// Check component in iterator, keep count of components in iterator
		int iterator_component_count = 0;
		ecs::components::TestComponent* pComponent;
		ecs::BaseComponent* pBase;
		while (pBase = it.next())
		{
			ASSERT_NE(pBase, nullptr);
			pComponent = static_cast<ecs::components::TestComponent*>(pBase);

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
namespace TestECSUser
{
	const int expected_event_data = 1111;
	const int expected_test_component_data = 2222;
	const int expected_other_component_data = 3333;

	#pragma region TestDataTypes
	class TestCreatorystem : public ecs::ECSSystem<TestCreatorystem>
	{
	public:

		TestCreatorystem()
		{
			updateType = ecs::SystemUpdateType::Actor;
		}
		virtual ~TestCreatorystem() {}
		void act(float _delta) override
		{
			// 1. Create entity with TestComponent
			// 2. Add OtherTestComponent to entity
			// 3. Create event

			ecs::Entity *p_entity = ecs::ECSUser::createEntity(TestComponent(expected_test_component_data));
			ASSERT_NE(p_entity, nullptr);

			OtherTestComponent* p_other = ecs::ECSUser::createComponent<OtherTestComponent>(p_entity->getID(), OtherTestComponent(expected_other_component_data));
			ASSERT_NE(p_other, nullptr);

			EXPECT_EQ(p_entity->getComponentCount(), 2);

			ecs::ECSUser::createEvent(TestEvent(expected_event_data));
		}
	};
	class TestGetterSystem : public ecs::ECSSystem<TestGetterSystem>
	{
	public:

		TestGetterSystem()
		{
			updateType = ecs::SystemUpdateType::Actor;
		}
		virtual ~TestGetterSystem() {}
		void act(float _delta) override
		{	
			// Pointers used to check getters
			ID entity_id;
			ID test_id;
			ecs::Entity *p_entity;
			ecs::BaseComponent *p_base;
			TestComponent *p_test;
			OtherTestComponent *p_other;

			// First try to get entity by using component types
			ecs::EntityIterator entity_iterator = ecs::ECSUser::getEntitiesWithComponent<TestComponent>();
			ASSERT_EQ(entity_iterator.entities.size(), 1);
			p_entity = entity_iterator.entities.front().entity;
			ASSERT_NE(p_entity, nullptr);

			// Store entity ID for later usage
			entity_id = p_entity->getID();

			// Try get with filter
			ecs::TypeFilter filter;
			filter.addRequirement(TestComponent::typeID);
			filter.addRequirement(OtherTestComponent::typeID);
			entity_iterator = ecs::ECSUser::getEntitiesByFilter(filter);
			ASSERT_EQ(entity_iterator.entities.size(), 1);
			p_entity = entity_iterator.entities.front().entity;
			ASSERT_NE(p_entity, nullptr);

			// Try get entity by ID
			p_entity = ecs::ECSUser::getEntity(entity_id);
			ASSERT_NE(p_entity, nullptr);

			// Store ID for the component
			test_id = p_entity->getComponentID<TestComponent>();
			
			// Try get component
			p_base = ecs::ECSUser::getComponent(TestComponent::typeID, test_id);
			p_test = ecs::ECSUser::getComponent<TestComponent>(test_id);
			EXPECT_NE(p_base, nullptr);
			EXPECT_NE(p_test, nullptr);

			// Through entity
			p_base = ecs::ECSUser::getComponentFromKnownEntity(TestComponent::typeID, entity_id);
			p_test = ecs::ECSUser::getComponentFromKnownEntity<TestComponent>(entity_id);
			EXPECT_NE(p_base, nullptr);
			EXPECT_NE(p_test, nullptr);

			// By iterator
			ecs::ComponentIterator component_iterator = ecs::ECSUser::getComponentsOfType(TestComponent::typeID);
			p_base = component_iterator.next();
			ASSERT_NE(p_base, nullptr);
			EXPECT_EQ(component_iterator.next(), nullptr); // Only one component are expected
			p_test = (TestComponent*)p_test;
			EXPECT_EQ(p_test->data, expected_test_component_data);
		}
	};
	class TestEventListenerSystem : public ecs::ECSSystem<TestEventListenerSystem>
	{
	public:
		bool isUpdated = false;
		TestEventListenerSystem()
		{
			updateType = ecs::SystemUpdateType::EventListenerOnly;
			subscribeEventCreation(TestEvent::typeID);
		}
		virtual ~TestEventListenerSystem() {}
		void onEvent(TypeID _eventType, ecs::BaseEvent* _event) override
		{
			isUpdated = true;
			if (_eventType == TestEvent::typeID)
			{
				TestEvent* p_event = (TestEvent*)_event;
				EXPECT_EQ(p_event->data, expected_event_data);
			}
		}
	};
	#pragma endregion TestDataTypes

	TEST(TestECSUser, CreaterGetterAndListener)
	{
		ecs::EntityComponentSystem ecs;

		// Create system through ecs, which initializes them for us.
		// Then, use the returning pointers to test the systems.
		TestCreatorystem *p_creatorSystem = ecs.createSystem<TestCreatorystem>();
		TestGetterSystem *p_getterSystem = ecs.createSystem<TestGetterSystem>();
		TestEventListenerSystem *p_listenerSystem = ecs.createSystem<TestEventListenerSystem>();

		// These act-methods has test expectations in them,
		p_creatorSystem->act(0.0f);
		p_getterSystem->act(0.0f);

		EXPECT_TRUE(p_listenerSystem->isUpdated);
	}
} // TestECSUser
#pragma endregion ECSUserTesting
#pragma region ECSTesting
namespace TestECS
{
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
			ecs::systems::TestEventListenerSystem			*p_elSystem  = ecs.createSystem<ecs::systems::TestEventListenerSystem>();
			ecs::systems::TestEntityUpdateSystem			*p_euSystem	 = ecs.createSystem<ecs::systems::TestEntityUpdateSystem>();
			ecs::systems::TestMultipleEntityUpdateSystem	*p_meuSystem = ecs.createSystem<ecs::systems::TestMultipleEntityUpdateSystem>();
			ecs::systems::TestEventReaderSystem				*p_erSystem  = ecs.createSystem<ecs::systems::TestEventReaderSystem>();
			ecs::systems::TestActorSystem					*p_aSystem	 = ecs.createSystem<ecs::systems::TestActorSystem>();

			ecs.createEntity(TestComponent(1337));
			ecs.createEvent(TestEvent(1337));

			// p_elSystem subscribes on TestEvent creation. Let's check that first.
			EXPECT_TRUE(p_elSystem->isUpdated);

			// Test all other system types
			// Create a map for expected update ticket number (order of execution)
			std::unordered_map<TypeID, int> expected_tickets;
			expected_tickets[ecs::systems::TestEntityUpdateSystem::typeID]			= 1;
			expected_tickets[ecs::systems::TestMultipleEntityUpdateSystem::typeID]	= 2;
			expected_tickets[ecs::systems::TestEventReaderSystem::typeID]			= 3;
			expected_tickets[ecs::systems::TestActorSystem::typeID]					= 4;

			// Set first ticket to be 1
			ecs::systems::g_update_ticket_counter = 1;

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

			ecs::Entity *p_entity = ecs.createEntity(TestComponent(1337));
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
			ecs::Entity *p_entity = ecs.createEntity(TestComponent(1337));

			ecs.removeEntity(p_entity->getID()); // Flags removal
			ecs.update(0.f); // Actual removals happen end of update

			EXPECT_EQ(ecs.getTotalEntityCount(), 0);
		}
		TEST(TestECSEntity, GetEntity)
		{
			ecs::EntityComponentSystem ecs;
			ID entity_id = ecs.createEntity(TestComponent(1337))->getID();

			ecs::Entity *p_entity = ecs.getEntity(entity_id);
			EXPECT_NE(p_entity, nullptr);

			ecs::TypeFilter filter;
			filter.addRequirement(TestComponent::typeID);
			ecs::EntityIterator it = ecs.getEntititesByFilter(filter);
			ASSERT_EQ(it.entities.size(), 1);
			EXPECT_EQ(it.entities.front().entity->getID(), entity_id);
		}
		TEST(TestECSEntity, RemoveLastComponentOfEntity)
		{
			/*
				Notation:
				An entity can't exist without a component. This test checks
				so that the entity is removed if it's last component is removed.
			*/

			ecs::EntityComponentSystem ecs;
			ecs::Entity *p_entity = ecs.createEntity(TestComponent(1337));
			ID component_id = p_entity->getComponentID<TestComponent>();

			ecs.removeComponent(p_entity->getID(), TestComponent::typeID);
			ecs.update(0.f);

			EXPECT_EQ(ecs.getTotalEntityCount(), 0);
		}
	} // TestECSEntity
	namespace TestECSComponent
	{
		TEST(TestECSComponent, CreateComponent)
		{
			ecs::EntityComponentSystem ecs;

			ecs::Entity *p_entity = ecs.createEntity(TestComponent(1337));
			
			EXPECT_EQ(ecs.getTotalComponentCount(), 1);

			OtherTestComponent *p_otherComp = ecs.createComponent<OtherTestComponent>(p_entity->getID(), OtherTestComponent(1337));
			ASSERT_NE(p_otherComp, nullptr);

			EXPECT_EQ(ecs.getTotalComponentCount(), 2);
			EXPECT_EQ(p_entity->getComponentCount(), 2);
		}
		TEST(TestECSComponent, RemoveComponent)
		{
			ecs::EntityComponentSystem ecs;

			ecs::Entity* p_entity = ecs.createEntity(TestComponent(1337), OtherTestComponent(2448));

			// Pre check
			EXPECT_EQ(ecs.getTotalComponentCount(), 2);
			ecs.removeComponent(p_entity->getID(), TestComponent::typeID);
			ecs.update(0.f);
			EXPECT_EQ(ecs.getTotalComponentCount(), 1);

			ecs.removeComponent(p_entity->getID(), OtherTestComponent::typeID);
			ecs.update(0.f);
			EXPECT_EQ(ecs.getTotalComponentCount(), 0);
		}
		TEST(TestECSComponent, GetComponent)
		{
			const int expected_test_data = 1337;
			const int expected_other_data = 2448;

			ecs::EntityComponentSystem ecs;
			ecs::Entity *p_entity = ecs.createEntity(TestComponent(expected_test_data), OtherTestComponent(expected_other_data));

			// Pointers for getting
			ecs::BaseComponent *p_base;
			TestComponent *p_test;
			OtherTestComponent *p_other;

			/*
				Test non-templated
			*/

			p_base = ecs.getComponent(TestComponent::typeID, p_entity->getComponentID(TestComponent::typeID));
			ASSERT_NE(p_base, nullptr);
			p_test = (TestComponent*)p_base;

			p_base = ecs.getComponent(OtherTestComponent::typeID, p_entity->getComponentID(OtherTestComponent::typeID));
			ASSERT_NE(p_base, nullptr);
			p_other = (OtherTestComponent*)p_base;

			EXPECT_EQ(p_test->data, expected_test_data);
			EXPECT_EQ(p_other->data, expected_other_data);

			/*
				Test templated
			*/

			p_base = ecs.getComponent<TestComponent>(p_entity->getComponentID<TestComponent>());
			ASSERT_NE(p_base, nullptr);
			p_test = (TestComponent*)p_base;

			p_base = ecs.getComponent<OtherTestComponent>(p_entity->getComponentID<OtherTestComponent>());
			ASSERT_NE(p_base, nullptr);
			p_other = (OtherTestComponent*)p_base;

			EXPECT_EQ(p_test->data, expected_test_data);
			EXPECT_EQ(p_other->data, expected_other_data);
		}
		TEST(TestECSComponent, Iterators)
		{
			const int expected_test_data = 1337;
			const int expected_other_data = 2448;

			ecs::EntityComponentSystem ecs;
			ecs::Entity* p_entity = ecs.createEntity(TestComponent(expected_test_data), OtherTestComponent(expected_other_data));

			// Pointers for getting
			ecs::BaseComponent* p_base;
			TestComponent* p_test;
			OtherTestComponent* p_other;

			ecs::ComponentIterator it = ecs.getAllComponentsOfType(TestComponent::typeID);
			p_base = it.next();
			ASSERT_NE(p_base, nullptr);
			p_test = (TestComponent*)p_base;
			EXPECT_EQ(p_test->data, expected_test_data);
			p_base = it.next();
			EXPECT_EQ(p_base, nullptr); // Expect iterator to only have one component

			it = ecs.getAllComponentsOfType(OtherTestComponent::typeID);
			p_base = it.next();
			ASSERT_NE(p_base, nullptr);
			p_other = (OtherTestComponent*)p_base;
			EXPECT_EQ(p_other->data, expected_other_data);
			p_base = it.next();
			EXPECT_EQ(p_base, nullptr); // Expect iterator to only have one component
		}
		TEST(TestECSComponent, CreateComponentAfterRemoval)
		{
			ecs::EntityComponentSystem ecs;
			ecs::Entity *p_entity = ecs.createEntity(TestComponent(1337), OtherTestComponent(1337));
			ecs.removeComponent(p_entity->getID(), TestComponent::typeID);
			ecs.update(0.f);

			// Pre check
			EXPECT_EQ(p_entity->getComponentID<TestComponent>(), 0);
			EXPECT_EQ(p_entity->getComponentCount(), 1);
			EXPECT_EQ(ecs.getTotalComponentCount(), 1);

			const int expected_component_data = 1111;
			TestComponent *p_component = ecs.createComponent<TestComponent>(p_entity->getID(), TestComponent(expected_component_data));

			ASSERT_NE(p_component, nullptr);
			EXPECT_EQ(p_component->data, expected_component_data);
			EXPECT_NE(p_entity->getComponentID<TestComponent>(), 0);
			EXPECT_EQ(p_entity->getComponentCount(), 2);
			EXPECT_EQ(ecs.getTotalComponentCount(), 2);
		}
	} // TestECSComponent
	namespace TestECSSystem
	{
		TEST(TestECSSystem, CreateSystem)
		{
			ecs::EntityComponentSystem ecs;

			ecs::systems::TestEntityUpdateSystem			*p_euSys;
			ecs::systems::TestMultipleEntityUpdateSystem	*p_meuSys;
			ecs::systems::TestEventReaderSystem				*p_erSys;
			ecs::systems::TestEventListenerSystem			*p_elSys;
			ecs::systems::TestActorSystem					*p_aSys;

			p_euSys		= ecs.createSystem<ecs::systems::TestEntityUpdateSystem>();
			p_meuSys	= ecs.createSystem<ecs::systems::TestMultipleEntityUpdateSystem>();
			p_erSys		= ecs.createSystem<ecs::systems::TestEventReaderSystem>();
			p_elSys		= ecs.createSystem<ecs::systems::TestEventListenerSystem>();
			p_aSys		= ecs.createSystem<ecs::systems::TestActorSystem>();

			EXPECT_EQ(ecs.getTotalSystemCount(), 5);

			EXPECT_NE(p_euSys, nullptr);
			EXPECT_NE(p_meuSys, nullptr);
			EXPECT_NE(p_erSys, nullptr);
			EXPECT_NE(p_elSys, nullptr);
			EXPECT_NE(p_aSys, nullptr);

			EXPECT_EQ(p_euSys->getTypeID(), ecs::systems::TestEntityUpdateSystem::typeID);
			EXPECT_EQ(p_meuSys->getTypeID(), ecs::systems::TestMultipleEntityUpdateSystem::typeID);
			EXPECT_EQ(p_erSys->getTypeID(), ecs::systems::TestEventReaderSystem::typeID);
			EXPECT_EQ(p_elSys->getTypeID(), ecs::systems::TestEventListenerSystem::typeID);
			EXPECT_EQ(p_aSys->getTypeID(), ecs::systems::TestActorSystem::typeID);

			EXPECT_NE(p_euSys->getFreeFunction(), nullptr);
			EXPECT_NE(p_meuSys->getFreeFunction(), nullptr);
			EXPECT_NE(p_erSys->getFreeFunction(), nullptr);
			EXPECT_NE(p_elSys->getFreeFunction(), nullptr);
			EXPECT_NE(p_aSys->getFreeFunction(), nullptr);

			EXPECT_NE(p_euSys->getName(), "");
			EXPECT_NE(p_meuSys->getName(), "");
			EXPECT_NE(p_erSys->getName(), "");
			EXPECT_NE(p_elSys->getName(), "");
			EXPECT_NE(p_aSys->getName(), "");
		}
		TEST(TestECSSystem, RemoveSystem)
		{
			ecs::EntityComponentSystem ecs;

			ecs::systems::TestEntityUpdateSystem			*p_euSys;
			ecs::systems::TestMultipleEntityUpdateSystem	*p_meuSys;
			ecs::systems::TestEventReaderSystem				*p_erSys;
			ecs::systems::TestEventListenerSystem			*p_elSys;
			ecs::systems::TestActorSystem					*p_aSys;

			p_euSys		= ecs.createSystem<ecs::systems::TestEntityUpdateSystem>();
			p_meuSys	= ecs.createSystem<ecs::systems::TestMultipleEntityUpdateSystem>();
			p_erSys		= ecs.createSystem<ecs::systems::TestEventReaderSystem>();
			p_elSys		= ecs.createSystem<ecs::systems::TestEventListenerSystem>();
			p_aSys		= ecs.createSystem<ecs::systems::TestActorSystem>();

			// Try remove systems
			ecs.removeSystem<ecs::systems::TestEntityUpdateSystem>();
			EXPECT_EQ(ecs.getTotalSystemCount(), 4);

			ecs.removeSystem<ecs::systems::TestMultipleEntityUpdateSystem>();
			EXPECT_EQ(ecs.getTotalSystemCount(), 3);

			ecs.removeSystem<ecs::systems::TestEventReaderSystem>();
			EXPECT_EQ(ecs.getTotalSystemCount(), 2);

			ecs.removeSystem<ecs::systems::TestEventListenerSystem>();
			EXPECT_EQ(ecs.getTotalSystemCount(), 1);

			ecs.removeSystem<ecs::systems::TestActorSystem>();
			EXPECT_EQ(ecs.getTotalSystemCount(), 0);

			// Try remove a non-existing system
			ecs.removeSystem<ecs::systems::TestEntityUpdateSystem>();
			EXPECT_EQ(ecs.getTotalSystemCount(), 0);
		}
		TEST(TestECSSystem, CreateAfterRemoveSystem)
		{
			ecs::EntityComponentSystem ecs;

			ecs::systems::TestEntityUpdateSystem			*p_euSys;
			ecs::systems::TestMultipleEntityUpdateSystem	*p_meuSys;
			ecs::systems::TestEventReaderSystem				*p_erSys;
			ecs::systems::TestEventListenerSystem			*p_elSys;
			ecs::systems::TestActorSystem					*p_aSys;

			p_euSys		= ecs.createSystem<ecs::systems::TestEntityUpdateSystem>();
			p_meuSys	= ecs.createSystem<ecs::systems::TestMultipleEntityUpdateSystem>();
			p_erSys		= ecs.createSystem<ecs::systems::TestEventReaderSystem>();
			p_elSys		= ecs.createSystem<ecs::systems::TestEventListenerSystem>();
			p_aSys		= ecs.createSystem<ecs::systems::TestActorSystem>();

			// Try remove systems
			ecs.removeSystem<ecs::systems::TestEntityUpdateSystem>();
			ecs.removeSystem<ecs::systems::TestMultipleEntityUpdateSystem>();
			ecs.removeSystem<ecs::systems::TestEventReaderSystem>();
			ecs.removeSystem<ecs::systems::TestEventListenerSystem>();
			ecs.removeSystem<ecs::systems::TestActorSystem>();

			// Pre check
			EXPECT_EQ(ecs.getTotalSystemCount(), 0);

			// Try create systems again
			p_euSys		= ecs.createSystem<ecs::systems::TestEntityUpdateSystem>();
			p_meuSys	= ecs.createSystem<ecs::systems::TestMultipleEntityUpdateSystem>();
			p_erSys		= ecs.createSystem<ecs::systems::TestEventReaderSystem>();
			p_elSys		= ecs.createSystem<ecs::systems::TestEventListenerSystem>();
			p_aSys		= ecs.createSystem<ecs::systems::TestActorSystem>();

			EXPECT_EQ(ecs.getTotalSystemCount(), 5);
			EXPECT_NE(p_euSys, nullptr);
			EXPECT_NE(p_meuSys, nullptr);
			EXPECT_NE(p_erSys, nullptr);
			EXPECT_NE(p_elSys, nullptr);
			EXPECT_NE(p_aSys, nullptr);
		}
	} // TestECSSystem
}
#pragma endregion ECSTesting