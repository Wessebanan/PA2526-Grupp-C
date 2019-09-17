#include "MovementSystem.h"

StaticMovementSystem::StaticMovementSystem()
{
	updateType = ecs::EntityUpdate;
	componentFilter.addRequirement(MovementComponent::typeID);
	// componentFilter.addRequirement(TransformComponent::typeID);

	subscribeEventCreation(MovementInputEvent::typeID);
}

StaticMovementSystem::~StaticMovementSystem()
{
}

void StaticMovementSystem::updateEntity(ecs::FilteredEntity& _entityInfo, float _delta)
{

}

