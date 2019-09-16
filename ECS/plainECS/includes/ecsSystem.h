#pragma once

#include <iostream>
#include "ecsBaseSystem.h"
#include "ecsComponent.h"
#include "ecsEvent.h"
#include "ecsTypeFilter.h"

using namespace std;
using namespace ecs::components;
using namespace ecs::events;

namespace ecs
{
	namespace systems
	{
		class PoisonSystem : public ECSSystem<PoisonSystem>
		{
		public:
			PoisonSystem()
			{
				updateType = EntityUpdate;
				componentFilter.addRequirement(HealthComponent::typeID);
				componentFilter.addRequirement(PoisonComponent::typeID);
			}
			~PoisonSystem() {}

			void updateEntity(FilteredEntity &_entityInfo, float _delta) override
			{
				HealthComponent* hp = _entityInfo.getComponent<HealthComponent>();
				PoisonComponent* poison = _entityInfo.getComponent<PoisonComponent>();

				cout << "[PoisonSystem] Entity " << _entityInfo.entity->getID() << " damaged by poison.";
				cout << " HP " << hp->health << " -> ";

				hp->health -= poison->tickDamage;

				cout << hp->health << " (" << poison->tickDamage << "dmg/tick)." << endl;
			}
		};

		class EntityKillSystem : public ECSSystem<EntityKillSystem>
		{
		public:
			EntityKillSystem()
			{
				updateType = EntityUpdate;
				componentFilter.addRequirement(HealthComponent::typeID);
			}
			~EntityKillSystem() {}

			void updateEntity(FilteredEntity &_entityInfo, float _delta) override
			{
				HealthComponent* hp = _entityInfo.getComponent<HealthComponent>();
				if (hp->health <= 0)
				{
					cout << "[EntityKillSystem] Entity " << _entityInfo.entity->getID() << " is dead." << endl;
					ECSUser::removeEntity(hp->getEntityID());
				}
			}
		};

		class EntityKillCounter : public ECSSystem<EntityKillCounter>
		{
		public:
			EntityKillCounter()
			{
				updateType = EventReader;
				eventFilter.addRequirement(RemoveEntityEvent::typeID);
			}
			~EntityKillCounter() {}

			void readEvent(BaseEvent& _event, float _delta) override
			{
				RemoveEntityEvent* e = (RemoveEntityEvent*)&_event;
				ID entityID = e->entityID;
				entityKills++;
				cout << "[EntityKillCounter] A total of " << entityKills << " entities has been killed so far." << endl;
			}

		private:
			size_t entityKills = 0;
		};

		class EventPrintingSystem : public ECSSystem<EventPrintingSystem>
		{
		public:
			EventPrintingSystem()
			{
				updateType = EventListenerOnly;
				subscribeEventCreation(events::CreateEntityEvent::typeID);
				subscribeEventCreation(events::RemoveEntityEvent::typeID);
				subscribeEventCreation(events::CreateComponentEvent::typeID);
				subscribeEventCreation(events::RemoveComponentEvent::typeID);
			}
			~EventPrintingSystem() {}

			void onEvent(TypeID _eventType, BaseEvent* _event) override
			{
				cout << "[EventPrintingSystem] ";

				if (_eventType == events::CreateEntityEvent::typeID)
				{
					events::CreateEntityEvent* e = static_cast<events::CreateEntityEvent*>(_event);
					cout << "Entity " << e->entityID << " has was born!" << endl;
				}
				else if (_eventType == events::RemoveEntityEvent::typeID)
				{
					events::RemoveEntityEvent* e = static_cast<events::RemoveEntityEvent*>(_event);
					cout << "Entity " << e->entityID << " has was removed!" << endl;
				}
				else if (_eventType == events::CreateComponentEvent::typeID)
				{
					events::CreateComponentEvent* e = static_cast<events::CreateComponentEvent*>(_event);
					BaseComponent* c = getComponent(e->componentTypeID, e->componentID);
					cout << "Entity " << e->entityID << " got component " << c->getName() << endl;
				}
				else if (_eventType == events::RemoveComponentEvent::typeID)
				{
					events::RemoveComponentEvent* e = static_cast<events::RemoveComponentEvent*>(_event);
					BaseComponent* c = getComponent(e->componentTypeID, e->componentID);
					cout << "Entity " << e->entityID << " lost component " << c->getName() << endl;
				}
			}
		};
	}
}