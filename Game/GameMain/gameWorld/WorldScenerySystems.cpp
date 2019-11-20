#include "WorldScenerySystems.h"
#include "WorldComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../Physics/PhysicsComponents.h"
#include "../Physics/PhysicsEvents.h"


namespace ecs
{
	namespace systems
	{
		WorldSceneryUpdateSystem::WorldSceneryUpdateSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::WorldSceneryComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);
		}

		WorldSceneryUpdateSystem::~WorldSceneryUpdateSystem()
		{
			//
		}

		void WorldSceneryUpdateSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
		{
			components::DynamicMovementComponent* kek = getComponentFromKnownEntity<components::DynamicMovementComponent>(_entityInfo.entity->getID());
			XMFLOAT3 temp_move(0.2f, 0, 0.2f);
			
			kek->mForward = temp_move;
			
			MovementInputEvent move;
			move.mInput = FORWARD;
			move.mEntityID = _entityInfo.entity->getID();
			createEvent(move);
		}
	}
}