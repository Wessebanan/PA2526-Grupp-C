#include "MovementSystem.h"

ecs::systems::StaticMovementSystem::StaticMovementSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(MovementComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
}

ecs::systems::StaticMovementSystem::~StaticMovementSystem()
{
}

void ecs::systems::StaticMovementSystem::updateEntity(ecs::FilteredEntity& _entityInfo, float _delta)
{
	// If an entity is movable and moving, move it and reduce velocity on update. 
	MovementComponent* movement = _entityInfo.getComponent<MovementComponent>();
	if (movement->mVelocity > 0.0f)
	{
		// Grabbing the transform component in order to modify the position of the entity.
		TransformComponent* transform = _entityInfo.getComponent<TransformComponent>();

		// The position is modified by the velocity multiplied by 
		// the update time and the direction in 3D. Assumes	that the 
		// direction vector is normalized. 
		// XMFLOAT3 has no operators besides = :joy: :gun:
		transform->position =  DirectX::XMFLOAT3(
			transform->position.x + movement->mVelocity * movement->mDirection.x,
			transform->position.y + movement->mVelocity * movement->mDirection.y,
			transform->position.z + movement->mVelocity * movement->mDirection.z);

		// Reducing the velocity by the constant deceleration value.
		movement->mVelocity -= DEFAULT_DECELERATION;
	}
}

ecs::systems::StaticMovementUpdateSystem::StaticMovementUpdateSystem()
{
	updateType = ecs::EventReader;
	eventTypes.addRequirement(MovementInputEvent::typeID);

	// Test
	subscribeEventCreation(MovementInputEvent::typeID);
}

ecs::systems::StaticMovementUpdateSystem::~StaticMovementUpdateSystem()
{
}



void ecs::systems::StaticMovementUpdateSystem::readEvent(ecs::BaseEvent& _event, float _delta)
{
	// Receiving a direction and entity ID from the event.
	MovementInputEvent input_event = dynamic_cast<MovementInputEvent&>(_event);
	
	// Finding the entity that should move and grabbing its movement component.
	Entity* entity_to_move = getEntity(input_event.mEntityID);
	ID movement_component_ID = entity_to_move->getComponentID(MovementComponent::typeID);
	MovementComponent* movement_component = dynamic_cast<MovementComponent*>(getComponent(MovementComponent::typeID, movement_component_ID));
	
	// Temporarily assuming forward is (1, 0, 0) until a better solution.
	// Would rather use camera forward as forward but we would need a camera for that.
	DirectX::XMFLOAT3 forward(1.0f, 0.0f, 0.0f);

	// Rotating its direction of movement based on current 
	// direction and input direction.
	float rotation = (float)input_event.mInput;

	// Because radians.
	float cos_rotation = (float)cos(rotation * PI / 180.0);
	float sin_rotation = (float)sin(rotation * PI / 180.0);

	// Readability temp variables.
	// float x = movement_component->mDirection.x;
	// float y = movement_component->mDirection.y;

	// Rotation application around z-axis.
	movement_component->mDirection.x = forward.x * cos_rotation - forward.y * sin_rotation;
	movement_component->mDirection.y = forward.x * sin_rotation + forward.y * cos_rotation;

	// Setting the rotated x and y to the
	// modified values.
	// movement_component->mDirection.x = x;
	// movement_component->mDirection.y = y;

	// Setting velocity to the default velocity.
	movement_component->mVelocity = DEFAULT_VELOCITY;
}

void ecs::systems::StaticMovementUpdateSystem::onEvent(TypeID _typeID, ecs::BaseEvent* _event)
{
	if (_typeID == MovementInputEvent::typeID)
	{
		std::cout << MovementInputEvent().getName() << std::endl;
	}

}

