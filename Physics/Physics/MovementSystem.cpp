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
			transform->position.x + movement->mVelocity * _delta * movement->mDirection.x,
			transform->position.y + movement->mVelocity * _delta * movement->mDirection.y,
			transform->position.z + movement->mVelocity * _delta * movement->mDirection.z);

		// If the moved entity has a bounding sphere, the 
		// collision system is notified with a potential
		// collision event.
		if (_entityInfo.components.find(BoundingSphereComponent::typeID) != _entityInfo.components.end())
		{
			PotentialCollisionEvent potential_collision;
			potential_collision.mEntityID = _entityInfo.entity->getID();
			createEvent(potential_collision);
		}

		// Removing the velocity, as there should be none unless there
		// is input saying otherwise.
		movement->mVelocity = 0;
	}
}

ecs::systems::StaticMovementUpdateSystem::StaticMovementUpdateSystem()
{
	updateType = ecs::EventReader;
	typeFilter.addRequirement(MovementInputEvent::typeID);
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

	// Rotating its direction of movement based on current 
	// direction and input direction.
	float rotation = (float)input_event.mInput;

	// Because radians.
	float cos_rotation = (float)cos(rotation * PI / 180.0);
	float sin_rotation = (float)sin(rotation * PI / 180.0);
	
	// Rotation application around y-axis using the forward vector of the movement component.
	movement_component->mDirection.x = movement_component->mForward.x * cos_rotation - movement_component->mForward.z * sin_rotation;
	movement_component->mDirection.z = movement_component->mForward.x * sin_rotation + movement_component->mForward.z * cos_rotation;

	// Setting velocity to the max velocity.
	movement_component->mVelocity = movement_component->mMaxVelocity;
}


