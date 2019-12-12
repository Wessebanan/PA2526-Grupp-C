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
			components::TrapComponent* p_trap_comp = trap.getComponent<components::TrapComponent>();
			components::TransformComponent* p_transf_comp = trap.getComponent<components::TransformComponent>();

			const GAME_OBJECT_TYPE TRAP_TYPE = p_trap_comp->mObjectType;
			const XMVECTOR trap_position = XMLoadFloat3(&p_transf_comp->position) + XMLoadFloat3(&TRIGGER_POINT_OFFSET);

			p_trap_comp->CurrentTimeInSeconds += delta;
			if (p_trap_comp->ActivationRateInSeconds <= p_trap_comp->CurrentTimeInSeconds)
			{
				p_trap_comp->CurrentTimeInSeconds = 0.0f;

				/*
					Trigger event
				*/
				TriggerTrapEvent trigger_event;
				trigger_event.unitID = 0;
				trigger_event.tileID = p_trap_comp->mTileID;
				trigger_event.trapID = trap.entity->getID();
				trigger_event.trapType = p_trap_comp->mObjectType;
				createEvent(trigger_event);
			}
		}
	}
}