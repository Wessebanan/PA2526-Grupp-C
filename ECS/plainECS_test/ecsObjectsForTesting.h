#pragma once

#include "ecsEventIncludes.h"
#include "ecsComponentIncludes.h"
#include "ecsSystemIncludes.h"

namespace ecs
{
	namespace events
	{
		struct TestEvent : public ecs::ECSEvent<TestEvent>
		{
			TestEvent(int _data) : data(_data) {}
			int data;
		};
	}

	namespace components
	{
		struct TestComponent : public ecs::ECSComponent<TestComponent>
		{
			TestComponent(int initData = -1) : data(initData) { flags = STATE_FLAG_VISIBLE; };
			int data;

			virtual ~TestComponent() { data = 0; }
		};

		struct OtherTestComponent : public ecs::ECSComponent<OtherTestComponent>
		{
			OtherTestComponent(int _data) : data(_data) { flags = STATE_FLAG_VISIBLE; }
			int data;

			virtual ~OtherTestComponent() { data = 0; }
		};
	}

	namespace systems
	{
		/*
			This counter is used to check in which order systems are
			updated. In each system's update, the system will fetch
			the current value in g_update_ticket_counter and increase
			it by one.
		*/
		static int g_update_ticket_counter;

		class TestEntityUpdateSystem : public ecs::ECSSystem<TestEntityUpdateSystem>
		{
		public:
			bool isUpdated = false;
			int updateTicket = 0;
			TestEntityUpdateSystem()
			{
				updateType = ecs::SystemUpdateType::EntityUpdate;
				typeFilter.addRequirement(components::TestComponent::typeID);
			}
			virtual ~TestEntityUpdateSystem() {}
			void updateEntity(ecs::FilteredEntity& _entityInfo, float _delta) override
			{
				isUpdated = true;
				updateTicket = g_update_ticket_counter++;
			}
		};
		class TestMultipleEntityUpdateSystem : public ecs::ECSSystem<TestMultipleEntityUpdateSystem>
		{
		public:
			bool isUpdated = false;
			int updateTicket = 0;
			TestMultipleEntityUpdateSystem()
			{
				updateType = ecs::SystemUpdateType::MultiEntityUpdate;
				typeFilter.addRequirement(components::TestComponent::typeID);
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
				typeFilter.addRequirement(events::TestEvent::typeID);
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
				subscribeEventCreation(events::TestEvent::typeID);
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

		static TypeID CREATOR_SYSTEM_TYPE_ID;
		class TestSystemRemoverSystem : public ecs::ECSSystem<TestSystemRemoverSystem>
		{
		public:
			TestSystemRemoverSystem()
			{
				updateType = ecs::SystemUpdateType::Actor;
			}
			virtual ~TestSystemRemoverSystem() {}

			void act(float _delta) override
			{
				RemoveSystem(CREATOR_SYSTEM_TYPE_ID);
			}
		};
		class TestSystemCreatorSystem : public ecs::ECSSystem<TestSystemCreatorSystem>
		{
		public:
			static TypeID GetSystemTypeID() { return TestSystemCreatorSystem::typeID; }

			TestSystemCreatorSystem()
			{
				updateType = ecs::SystemUpdateType::Actor;
				CREATOR_SYSTEM_TYPE_ID = TestSystemCreatorSystem::typeID;
			}
			virtual ~TestSystemCreatorSystem() {}

			void act(float _delta) override
			{
				CreateSystem<TestSystemRemoverSystem>(0);
			}
		};
	}
}