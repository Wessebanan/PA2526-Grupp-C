#pragma once
#include "ecs.h"
#include "Components.h"
#include "Events.h"

#define SYSTEM(name) struct name : public ecs::ECSSystem<name>

//SYSTEM(StaticMovementSystem)
struct StaticMovementSystem : public ecs::ECSSystem<StaticMovementSystem>
{
	// TODO: Handle input from input handler.
	StaticMovementSystem()
	{
		updateType = ecs::EntityUpdate;
		componentFilter.addRequirement(MovementComponent::typeID);
		subscribeEventCreation(MovementInputEvent::typeID);
	}
	virtual ~StaticMovementSystem()
	{
	}
	/*void updateEntity() override
	{

	}*/
};