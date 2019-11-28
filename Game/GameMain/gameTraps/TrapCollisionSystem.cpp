#include "TrapCollisionSystem.h"
#include "TrapEvents.h"
#include "TrapComponents.h"
#include "../gameUtility/UtilityComponents.h" // TransformComponent
#include "../gameAI/AIComponents.h"	// UnitComponent
#include "../gameAI/AISystems.h"	// GetCloseset tiel

using namespace DirectX;

namespace ecs
{
	namespace systems
	{
		TrapCollisionSystem::TrapCollisionSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::TrapComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
		}

		TrapCollisionSystem::~TrapCollisionSystem()
		{
			//
		}

		void TrapCollisionSystem::updateEntity(FilteredEntity& trap, float delta)
		{
			//components::TrapComponent* p_trap_comp = trap.getComponent<components::TrapComponent>();
			//components::TransformComponent* p_transf_comp = trap.getComponent<components::TransformComponent>();

			//const GAME_OBJECT_TYPE TRAP_TYPE = p_trap_comp->mObjectType;
			//const XMVECTOR trap_position = XMLoadFloat3(&p_transf_comp->position) + XMLoadFloat3(&TRIGGER_POINT_OFFSET);

			//// If this sets to true, the trap will be deleted at end of update.
			//bool has_been_triggered = false;

			///*
			//	Retrieve all existing units
			//*/

			//TypeFilter unit_filter;
			//unit_filter.addRequirement(components::UnitComponent::typeID);
			//unit_filter.addRequirement(components::TransformComponent::typeID);
			//EntityIterator units = getEntitiesByFilter(unit_filter);

			///*
			//	Iterate all units and check their distane to the trap. If they are within
			//	TRIGGER_DISTANCE, create event for trap handler and remove trap.
			//*/

			//XMVECTOR unit_position;
			//for (FilteredEntity& unit : units.entities)
			//{
			//	unit_position = XMLoadFloat3(&unit.getComponent<components::TransformComponent>()->position);

			//	/*
			//		If unit is out of reach of the trap, continue to check next unit.
			//	*/

			//	if (XMVectorGetX(XMVector3Length(unit_position - trap_position)) > TRIGGER_DISTANCE)
			//	{
			//		continue;
			//	}

			//	/*
			//		Trigger event
			//	*/

			//	has_been_triggered = true;

			//	switch (TRAP_TYPE)
			//	{
			//	case GAME_OBJECT_TYPE_TRAP_FIRE:
			//	{
			//		events::TriggerFireTrapEvent fire_event;
			//		fire_event.unitID = unit.entity->getID();
			//		createEvent(fire_event);
			//		break;
			//	}

			//	case GAME_OBJECT_TYPE_TRAP_FREEZE:
			//	{
			//		events::TriggerFreezeTrapEvent freeze_event;
			//		freeze_event.unitID = unit.entity->getID();
			//		createEvent(freeze_event);
			//		break;
			//	}

			//	case GAME_OBJECT_TYPE_TRAP_SPRING:
			//	{
			//		events::TriggerSpringTrapEvent spring_event;
			//		spring_event.unitID = unit.entity->getID();
			//		spring_event.tileID = p_trap_comp->mTileID;
			//		createEvent(spring_event);
			//		break;
			//	}

			//	case GAME_OBJECT_TYPE_TRAP_SPIKES:
			//	{
			//		events::TriggerSpikeTrapEvent spike_event;
			//		spike_event.unitID = unit.entity->getID();
			//		createEvent(spike_event);
			//		break;
			//	}
			//	}
			//}

			///*
			//	Remove trap if triggered
			//*/

			////if (has_been_triggered)
			////{
			////	removeEntity(trap.entity->getID());
			////}

			components::TrapComponent* p_trap_comp = trap.getComponent<components::TrapComponent>();
			components::TransformComponent* p_transf_comp = trap.getComponent<components::TransformComponent>();

			const GAME_OBJECT_TYPE TRAP_TYPE = p_trap_comp->mObjectType;
			const XMVECTOR trap_position = XMLoadFloat3(&p_transf_comp->position) + XMLoadFloat3(&TRIGGER_POINT_OFFSET);

			/*
				Retrieve all existing units
			*/


			p_trap_comp->CurrentTimeInSeconds += delta;
			if (p_trap_comp->ActivationRateInSeconds <= p_trap_comp->CurrentTimeInSeconds)
			{
				p_trap_comp->CurrentTimeInSeconds = 0.0f;

				/*
					Trigger event
				*/

				switch (TRAP_TYPE)
				{
				case GAME_OBJECT_TYPE_TRAP_FIRE:
					{
						events::TriggerFireTrapEvent fire_event;
						fire_event.unitID = 0;
						fire_event.tileID = p_trap_comp->mTileID;
						fire_event.trapID = trap.entity->getID();
						createEvent(fire_event);

					} break;

				case GAME_OBJECT_TYPE_TRAP_FREEZE:
					{
						events::TriggerFreezeTrapEvent freeze_event;
						freeze_event.unitID = 0;
						freeze_event.tileID = p_trap_comp->mTileID;
						freeze_event.trapID = trap.entity->getID();
						createEvent(freeze_event);
						
					} break;

				case GAME_OBJECT_TYPE_TRAP_SPRING:
					{
						events::TriggerSpringTrapEvent spring_event;
						spring_event.unitID = 0;
						spring_event.tileID = p_trap_comp->mTileID;
						spring_event.trapID = trap.entity->getID();
						createEvent(spring_event);
						
					} break;

				case GAME_OBJECT_TYPE_TRAP_SPIKES:
					{
						events::TriggerSpikeTrapEvent spike_event;
						spike_event.unitID = 0;
						spike_event.tileID = p_trap_comp->mTileID;
						spike_event.trapID = trap.entity->getID();
						createEvent(spike_event);
						
					} break;
				}
			}
		}
	}
}