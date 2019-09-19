#include "pch.h"
#include "InputTest.h"

ecs::systems::InputSystem::InputSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(MovementComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
}

ecs::systems::InputSystem::~InputSystem()
{
}

void ecs::systems::InputSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	static int input_counter = 0;
	MovementInputs inputs[] = { FORWARD, BACKWARD, RIGHT, LEFT };

	// Changes direction every update.
	MovementInputEvent movement_event;
	movement_event.mEntityID = _entityInfo.entity->getID();
	movement_event.mInput = inputs[input_counter % 4];

	createEvent(movement_event);
	input_counter++;
}