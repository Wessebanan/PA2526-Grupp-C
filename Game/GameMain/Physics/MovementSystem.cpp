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

		StaticMove(transform->position, movement->mDirection, movement->mVelocity, _delta);

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
	SetDirection(movement_component->mDirection, movement_component->mForward, input_event.mInput);

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

	UpdateAcceleration(movement_component->mAcceleration, movement_component->mForce, movement_component->mWeight, movement_component->mVelocity, movement_component->mDeceleration);

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
	
	// If the object is not grounded right now, apply gravity. Else reset velocity.
	if (!movement_component->mOnGround)
	{
		HandleGravity(movement_component->mAcceleration.y, movement_component->mVelocity.y, transform_component->position.y, movement_component->mGravity, movement_component->mMaxVelocity, _delta);
	}
	else
	{
		movement_component->mVelocity.y = 0.0f;
	}

	// Potential collision if object moved.
	const float ABS_ERROR = (float)pow(10.0, -10.0);

	// If the difference after movement is more than negligible...
	if (CalculateDistance(transform_component->position, movement_component->mPreviousPos) > ABS_ERROR &&
		_entityInfo.entity->hasComponentOfType(ObjectCollisionComponent::typeID))
	{
		// ...make a potential collision event.
		PotentialCollisionEvent potential_collision;
		potential_collision.mEntityID = _entityInfo.entity->getID();
		potential_collision.mDelta = _delta;
		createEvent(potential_collision);
	}
	movement_component->mPreviousPos = transform_component->position;
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

	SetDirection(movement_component->mDirection, movement_component->mForward, input_event.mInput);

	// Applying force in the direction of the movement.
	movement_component->mForce.x = movement_component->mDirection.x * movement_component->mMovementForce;
	movement_component->mForce.z = movement_component->mDirection.z * movement_component->mMovementForce;
}
#pragma endregion