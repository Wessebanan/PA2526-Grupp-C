#pragma once
#include "ecs.h"
#include "Components.h"
#include "Events.h"

#define SYSTEM(name) struct name : public ecs::ECSSystem<name>

//SYSTEM(StaticMovementSystem)
SYSTEM(StaticMovementSystem)
{
	// TODO: Handle input from input handler.
	StaticMovementSystem()
	{
		updateType = ecs::EntityUpdate;
		componentFilter.addRequirement(MovementComponent::typeID);
		// componentFilter.addRequirement(PositionComponent::typeID);

		subscribeEventCreation(MovementInputEvent::typeID);
	}
	virtual ~StaticMovementSystem()
	{
	}
	void updateEntity(ecs::FilteredEntity& _entityInfo, float _delta) override
	{

	}
};