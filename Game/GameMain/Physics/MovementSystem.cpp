#include "MovementSystem.h"
#include "..//gameAI/AIComponents.h"

#pragma region DynamicMovementSystem
ecs::systems::DynamicMovementSystem::DynamicMovementSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(DynamicMovementComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
	subscribeEventCreation(MovementInputEvent::typeID);
	subscribeEventCreation(ForceImpulseEvent::typeID);
}

ecs::systems::DynamicMovementSystem::~DynamicMovementSystem()
{

}

void ecs::systems::DynamicMovementSystem::updateEntity(ecs::FilteredEntity& _entityInfo, float _delta)
{
	DynamicMovementComponent* movement_component = getComponentFromKnownEntity<DynamicMovementComponent>(_entityInfo.entity->getID());
	TransformComponent* transform_component = getComponentFromKnownEntity<TransformComponent>(_entityInfo.entity->getID());
	UnitScalePercent* unit_scale = getComponentFromKnownEntity<UnitScalePercent>(_entityInfo.entity->getID());

	// WALKING

	// No walking if unit is not on ground.
	if (movement_component->mOnGround)
	{
		// a = F/m
		movement_component->mAcceleration.x = movement_component->mInputForce.x / movement_component->mWeight;
		movement_component->mAcceleration.z = movement_component->mInputForce.z / movement_component->mWeight;

		// Applying deceleration if velocity is greater than 0 and acceleration works opposite or not at all, or if not on ground.
		if (fabs(movement_component->mVelocity.x) > 0.001f && movement_component->mAcceleration.x / movement_component->mVelocity.x <= 0.0f)
		{
			movement_component->mAcceleration.x -= Sign(movement_component->mVelocity.x) * movement_component->mDeceleration;
		}
		if (fabs(movement_component->mVelocity.z) > 0.001f && movement_component->mAcceleration.z / movement_component->mVelocity.z <= 0.0f)
		{
			movement_component->mAcceleration.z -= Sign(movement_component->mVelocity.z) * movement_component->mDeceleration;
		}

		// Save direction of velocity prior to change.
		int prev_sign_x = Sign(movement_component->mVelocity.x);
		int prev_sign_z = Sign(movement_component->mVelocity.z);

		// v = v_0 + a*delta_t
		movement_component->mVelocity.x += movement_component->mAcceleration.x * _delta;
		movement_component->mVelocity.z += movement_component->mAcceleration.z * _delta;
		
		// If this velocity change changed the sign of the velocity and there was no input: reset velocity to 0.
		if (prev_sign_x != Sign(movement_component->mVelocity.x) && fabs(movement_component->mInputForce.x) < 0.01f)
		{
			movement_component->mVelocity.x = 0.0f;
		}
		if (prev_sign_z != Sign(movement_component->mVelocity.z) && fabs(movement_component->mInputForce.z) < 0.01f)
		{
			movement_component->mVelocity.z = 0.0f;
		}

		// If the max velocity is exceeded, normalize velocity and scale by max velocity but disregard y.
		if (CalculateVectorLength(movement_component->mVelocity) > movement_component->mMaxVelocity)
		{
			XMStoreFloat3(&movement_component->mVelocity, XMVector3Normalize(XMLoadFloat3(&movement_component->mVelocity)));
			movement_component->mVelocity.x *= movement_component->mMaxVelocity;
			movement_component->mVelocity.z *= movement_component->mMaxVelocity;
		}
	}

	// GRAVITY
	
	// If the object is not grounded right now, apply gravity. Else reset velocity.
	if (!movement_component->mOnGround)
	{
		movement_component->mAcceleration.y = -movement_component->mGravity;		
		movement_component->mVelocity.y += movement_component->mAcceleration.y * _delta;		
	}
	else
	{
		movement_component->mVelocity.y = 0.0f;
	}

	// OTHER FORCES 

	// Other forces section exists because walking is strictly capped to max velocity but some shit
	// like knockback and spring traps shouldn't be affected by how fast units can walk.

	// If there are other forces.
	if (CalculateVectorLength(movement_component->mForce) > 0.01f)
	{
		// Accelerate unit by force and apply acceleration to unit velocity.
		movement_component->mAcceleration.x = movement_component->mForce.x / movement_component->mWeight;
		movement_component->mAcceleration.y = movement_component->mForce.y / movement_component->mWeight;
		movement_component->mAcceleration.z = movement_component->mForce.z / movement_component->mWeight;
		
		movement_component->mVelocity.x += movement_component->mAcceleration.x;
		movement_component->mVelocity.y += movement_component->mAcceleration.y;
		movement_component->mVelocity.z += movement_component->mAcceleration.z;
	}

	// d = d_0 + v*delta_t
	transform_component->position.x += movement_component->mVelocity.x * _delta;
	transform_component->position.y += movement_component->mVelocity.y * _delta;
	transform_component->position.z += movement_component->mVelocity.z * _delta;

	//If the unit is bellow -0.9 in y we know that the unit is on a water tile. Kill the unit.
	if (unit_scale != nullptr && transform_component->position.y - transform_component->scale.y * unit_scale->UnitScale <= -1.0f && _entityInfo.entity->hasComponentOfType<UnitComponent>())
	{
		DeadComponent dead_comp;
		dead_comp.cause = DeadComponent::CAUSE_DROWNING;
		dead_comp.position = transform_component->position;
		ecs::ECSUser::createComponent(transform_component->getEntityID(), dead_comp);
	}
	
	// Potential collision if object moved.
	const float ABS_ERROR = (float)pow(10.0, -10.0);
	
	// If movement happened in y, no longer on ground.
	if (fabs(movement_component->mPreviousPos.y - transform_component->position.y) > ABS_ERROR)
	{
		movement_component->mOnGround = false;
	}

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
	

	// Reset forces to 0 after since it should be 0 if no further input.
	movement_component->mInputForce = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	movement_component->mForce		= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	// Saving previous position for later comparison.
	movement_component->mPreviousPos = transform_component->position;
}

void ecs::systems::DynamicMovementSystem::onEvent(TypeID _typeID, ecs::BaseEvent* _event)
{
	// Casting the event to each of the interesting types.
	MovementInputEvent *input_event = dynamic_cast<MovementInputEvent*>(_event);
	ForceImpulseEvent *impulse_event = dynamic_cast<ForceImpulseEvent*>(_event);

	if (input_event)
	{
		// Checking that the entity should move dynamically.
		Entity* entity = getEntity(input_event->mEntityID);
		if (!entity->hasComponentOfType(DynamicMovementComponent::typeID))
		{
			return;
		}

		// Apply force in the direction of the movement input relative to forward direction.
		MovementInputs input = input_event->mInput;

		DynamicMovementComponent* movement_component = getComponentFromKnownEntity<DynamicMovementComponent>(entity->getID());

		// No movement input if unit is not on ground.
		if (!movement_component->mOnGround)
		{
			return;
		}

		SetDirection(movement_component->mDirection, movement_component->mForward, input_event->mInput);

		// Applying force in the direction of the movement.
		movement_component->mInputForce.x = movement_component->mDirection.x * movement_component->mMovementForce;
		movement_component->mInputForce.z = movement_component->mDirection.z * movement_component->mMovementForce;

		// Rotate entity to face the same direction as movement.
		TransformComponent* transform_component = getComponentFromKnownEntity<TransformComponent>(entity->getID());

		// NOTE: No way of finding default direction of mesh so it's hard coded.
		XMFLOAT3 dude_default_forward = XMFLOAT3(0.0f, 0.0f, -1.0f);
		XMFLOAT3* movement_forward = &movement_component->mForward;

		// Finding the angle between default forward and movement forward direction.
		XMVECTOR cos_angle = DirectX::XMVector3Dot(XMLoadFloat3(&dude_default_forward), XMLoadFloat3(movement_forward));

		// Using Sign to find if the angle is negative or positive relative to default forward.
		transform_component->rotation.y = -Sign(movement_forward->x) * acos(XMVectorGetX(cos_angle));
	}
	else if (impulse_event)
	{
		// Checking that the entity should move dynamically.
		Entity* entity = getEntity(impulse_event->mEntityID);
		if (!entity->hasComponentOfType(DynamicMovementComponent::typeID))
		{
			return;
		}

		DynamicMovementComponent* movement_component = getComponentFromKnownEntity<DynamicMovementComponent>(entity->getID());
		movement_component->mForce.x = impulse_event->mDirection.x * impulse_event->mForce;
		movement_component->mForce.y = impulse_event->mDirection.y * impulse_event->mForce;
		movement_component->mForce.z = impulse_event->mDirection.z * impulse_event->mForce;
	}
}
#pragma endregion
#pragma region DynamicMovementInitSystem
ecs::systems::DynamicMovementInitSystem::DynamicMovementInitSystem()
{
	updateType = EventListenerOnly;
	subscribeEventCreation(CreateComponentEvent::typeID);
}
ecs::systems::DynamicMovementInitSystem::~DynamicMovementInitSystem()
{

}
void ecs::systems::DynamicMovementInitSystem::onEvent(TypeID _typeID, ecs::BaseEvent* _event)
{
	CreateComponentEvent* create_component_event = dynamic_cast<CreateComponentEvent*>(_event);

	// If the component created was any other than dynamic movement component, do nothing.
	if (create_component_event->componentTypeID != DynamicMovementComponent::typeID)
	{
		return;
	}

	ID entity_id = create_component_event->entityID;
	DynamicMovementComponent* movement_component = getComponentFromKnownEntity<DynamicMovementComponent>(entity_id);
	TransformComponent* transform_component = getComponentFromKnownEntity<TransformComponent>(entity_id);
}
#pragma endregion

#pragma region ThrowEventSystem
ecs::systems::ThrowEventSystem::ThrowEventSystem()
{
	updateType = EventListenerOnly;
	subscribeEventCreation(events::ThrowUnitEvent::typeID);
}
ecs::systems::ThrowEventSystem::~ThrowEventSystem()
{

}
void ecs::systems::ThrowEventSystem::onEvent(TypeID _typeID, ecs::BaseEvent* _event)
{
	if (_event->getTypeID() != ThrowUnitEvent::typeID)	return;
	

	ThrowUnitEvent* p_event = dynamic_cast<ThrowUnitEvent*>(_event);

	ID unit_id = p_event->mUnitID;
	ID tile_id = p_event->mTileID;
	//DynamicMovementComponent* movement_component = getComponentFromKnownEntity<DynamicMovementComponent>(unit_id);
	UnitComponent* unit_component = getComponentFromKnownEntity<UnitComponent>(unit_id);
	TransformComponent* unit_transform_component = getComponentFromKnownEntity<TransformComponent>(unit_id);
	TransformComponent* tile_transform_component = getComponentFromKnownEntity<TransformComponent>(tile_id);

	events::ForceImpulseEvent force_eve;
	force_eve.mEntityID = unit_id;
	//unit_transform_component->position.y += 1.5f;

	XMFLOAT3 flight_direction;
	XMFLOAT3 flight_direction_norm;
	flight_direction.x = tile_transform_component->position.x - unit_transform_component->position.x;
	flight_direction.y = 0.0f;
	flight_direction.z = tile_transform_component->position.z - unit_transform_component->position.z;

	// normalize the direction
	XMStoreFloat3(&flight_direction_norm, XMVector3Normalize(XMLoadFloat3(&flight_direction)));
	flight_direction_norm.y = 1.0f;
	force_eve.mDirection = flight_direction_norm;

	// get the distance of the flight
	float dist = sqrtf((flight_direction.x * flight_direction.x) + (flight_direction.z * flight_direction.z));
	float s = dist;
	float t = 2.0f;
	float v = s / t;

	float weight = DEFAULT_WEIGHT;
	force_eve.mForce = v * weight;
	force_eve.mDirection.y = (9.82f * weight) / (force_eve.mForce);


	createEvent(force_eve);
}
#pragma endregion