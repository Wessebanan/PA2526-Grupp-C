#include "MovementSystem.h"

#pragma region StaticMovementSystem
ecs::systems::StaticMovementSystem::StaticMovementSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(StaticMovementComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
}

ecs::systems::StaticMovementSystem::~StaticMovementSystem()
{
}

void ecs::systems::StaticMovementSystem::updateEntity(ecs::FilteredEntity& _entityInfo, float _delta)
{
	// If an entity is movable and moving, move it and reduce velocity on update. 
	StaticMovementComponent* movement = _entityInfo.getComponent<StaticMovementComponent>();
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
#pragma endregion
#pragma region StaticMovementUpdateSystem
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

	// Checking that the entity should move statically.
	Entity* entity = getEntity(input_event.mEntityID);
	if (!entity->hasComponentOfType(StaticMovementComponent::typeID))
	{
		return;
	}

	// Finding the entity that should move and grabbing its movement component.
	Entity* entity_to_move = getEntity(input_event.mEntityID);
	ID movement_component_ID = entity_to_move->getComponentID(StaticMovementComponent::typeID);
	StaticMovementComponent* movement_component = dynamic_cast<StaticMovementComponent*>(getComponent(StaticMovementComponent::typeID, movement_component_ID));

	// Rotating its direction of movement based on current 
	// direction and input direction.
	DirectX::XMFLOAT3 direction = movement_component->mForward;
	RotateAroundY(direction, input_event.mInput);

	movement_component->mDirection.x = direction.x;
	movement_component->mDirection.z = direction.z;

	// Setting velocity to the max velocity.
	movement_component->mVelocity = movement_component->mMaxVelocity;
}
#pragma endregion
#pragma region DynamicMovementSystem
ecs::systems::DynamicMovementSystem::DynamicMovementSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(DynamicMovementComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
	subscribeEventCreation(MovementInputEvent::typeID);
}

ecs::systems::DynamicMovementSystem::~DynamicMovementSystem()
{

}

void ecs::systems::DynamicMovementSystem::updateEntity(ecs::FilteredEntity& _entityInfo, float _delta)
{
	DynamicMovementComponent* movement_component = getComponentFromKnownEntity<DynamicMovementComponent>(_entityInfo.entity->getID());
	TransformComponent* transform_component = getComponentFromKnownEntity<TransformComponent>(_entityInfo.entity->getID());

	// Starting off with movement in the x-z-plane.

	// a = F/m
	movement_component->mAcceleration.x = movement_component->mForce.x / movement_component->mWeight;
	movement_component->mAcceleration.z = movement_component->mForce.z / movement_component->mWeight;
	
	// Applying deceleration if velocity is greater than 0 and acceleration works opposite or not at all.
	if (fabs(movement_component->mVelocity.x) > 0.0f && movement_component->mAcceleration.x / movement_component->mVelocity.x <= 0.0f)
	{
		// Reducing velocity by acceleration and time in the opposite direction of the velocity.
		movement_component->mAcceleration.x -= Sign(movement_component->mVelocity.x) * DEFAULT_DECELERATION;
	}
	if (fabs(movement_component->mVelocity.z) > 0.0f && movement_component->mAcceleration.z / movement_component->mVelocity.z <= 0.0f)
	{
		// Reducing velocity by acceleration and time in the opposite direction of the velocity.
		movement_component->mAcceleration.z -= Sign(movement_component->mVelocity.z) * DEFAULT_DECELERATION;
	}

	// If the max velocity is not exceeded:
	if (CalculateVectorLength(movement_component->mVelocity) < movement_component->mMaxVelocity)
	{
		// v = v_0 + a*delta_t
		movement_component->mVelocity.x += movement_component->mAcceleration.x * _delta;
		movement_component->mVelocity.z += movement_component->mAcceleration.z * _delta;
	}

	// d = d_0 + v*delta_t
	transform_component->position.x += movement_component->mVelocity.x * _delta;
	transform_component->position.z += movement_component->mVelocity.z * _delta;

	// Reset movement force to 0 after since it should be 0 if no further input.
	movement_component->mForce = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	// GRAVITY
	
	movement_component->mAcceleration.y = -GRAVITY;
	if (fabs(movement_component->mVelocity.y) < movement_component->mMaxVelocity)
	{
		movement_component->mVelocity.y += movement_component->mAcceleration.y * _delta;
	}

	// Separate if rather than else for the specific moment max velocity is passed.
	if (fabs(movement_component->mVelocity.y) > movement_component->mMaxVelocity)
	{
		movement_component->mVelocity.y = Sign(movement_component->mVelocity.y) * movement_component->mMaxVelocity;
	}
	transform_component->position.y += movement_component->mVelocity.y * _delta;
}

void ecs::systems::DynamicMovementSystem::onEvent(TypeID _typeID, ecs::BaseEvent* _event)
{
	MovementInputEvent input_event = dynamic_cast<MovementInputEvent&>(*_event);

	// Checking that the entity should move dynamically.
	Entity* entity = getEntity(input_event.mEntityID);
	if (!entity->hasComponentOfType(DynamicMovementComponent::typeID))
	{
		return;
	}

	// Apply force in the direction of the movement input relative to forward direction.
	MovementInputs input = input_event.mInput;

	DynamicMovementComponent* movement_component = getComponentFromKnownEntity<DynamicMovementComponent>(entity->getID());

	// Rotating direction of movement around the y axis.
	DirectX::XMFLOAT3 direction = movement_component->mForward;
	RotateAroundY(direction, input_event.mInput);

	// Updating direction according to the direction of movement.
	movement_component->mDirection.x = direction.x;
	movement_component->mDirection.z = direction.z;

	// Applying force in the direction of the movement.
	movement_component->mForce.x = direction.x * DEFAULT_MOVEMENT_FORCE;
	movement_component->mForce.z = direction.z * DEFAULT_MOVEMENT_FORCE;
}
#pragma endregion