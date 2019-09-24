#pragma once
#include "pch.h"
#include "MovementSystem.h"
#include "InputTest.h"

namespace MovementLogic
{
	TEST(MovementLogic, CreateStaticMovementSystem)
	{
		ecs::EntityComponentSystem ecs;
		ecs::systems::StaticMovementSystem* static_movement_system = ecs.createSystem<ecs::systems::StaticMovementSystem>();

		// Checking that ecs has one system at this point.
		EXPECT_EQ(ecs.getTotalSystemCount(), 1);

		// Checking that the pointer to the system is not equal to nullptr.
		EXPECT_NE(static_movement_system, nullptr);
	}

	TEST(MovementLogic, CreateStaticMovementUpdateSystem)
	{
		ecs::EntityComponentSystem ecs;

		// Checking that ecs has no systems at this point.
		EXPECT_EQ(ecs.getTotalSystemCount(), 0);

		ecs::systems::StaticMovementUpdateSystem* static_movement_update_system = ecs.createSystem<ecs::systems::StaticMovementUpdateSystem>();

		// Checking that ecs has one system at this point.
		EXPECT_EQ(ecs.getTotalSystemCount(), 1);

		// Checking that the pointer to the system is not equal to nullptr.
		EXPECT_NE(static_movement_update_system, nullptr);
	}

	TEST(MovementLogic, CreateMovableEntity)
	{
		ecs::EntityComponentSystem ecs;

		// Making sure there are no entities or components.
		EXPECT_EQ(ecs.getTotalEntityCount(), 0);
		EXPECT_EQ(ecs.getTotalComponentCount(), 0);

		MovementComponent movement;
		TransformComponent transform;

		ecs::Entity* movable_entity = ecs.createEntity(movement, transform);

		// Checking that there is now one entity and two components.
		EXPECT_EQ(ecs.getTotalEntityCount(), 1);
		EXPECT_EQ(ecs.getTotalComponentCount(), 2);

		// Checking that the pointer received from getEntity matches
		// the pointer to the entity.
		EXPECT_EQ(ecs.getEntity(movable_entity->getID()), movable_entity);
	}

	TEST(MovementLogic, HandleInput)
	{
		ecs::EntityComponentSystem ecs;

		// Temporary system to see if movement systems handle inputs correctly.
		// Layer 0 to ensure input is handled first.
		ecs.createSystem<ecs::systems::InputSystem>(0);

		ecs.createSystem<ecs::systems::StaticMovementUpdateSystem>();
		ecs.createSystem<ecs::systems::StaticMovementSystem>();
		
		MovementComponent movement;
		TransformComponent transform;

		ecs::Entity* movable_entity = ecs.createEntity(movement, transform);

		// Grabbing pointers to the components modified by the systems.
		MovementComponent* p_movement = dynamic_cast<MovementComponent*>(ecs.getComponent(MovementComponent::typeID, movable_entity->getComponentID(MovementComponent::typeID)));
		TransformComponent* p_transform = dynamic_cast<TransformComponent*>(ecs.getComponent(TransformComponent::typeID, movable_entity->getComponentID(TransformComponent::typeID)));

		const float DELTA = 0.1f;
		const float abs_error = (float)pow(10, -10);
		// Saving the previous position to check correct movement.
		DirectX::XMFLOAT3 previous_position = p_transform->position;

		// Each update gives a new input event with 
		// the max velocity and a different direction each time
		// in the order: [FORWARD, BACKWARD, RIGHT, LEFT].

		// FORWARD
		ecs.update(DELTA);
		
		// Checking direction.
		EXPECT_NEAR(p_movement->mDirection.x, p_movement->mForward.x, abs_error);
		EXPECT_NEAR(p_movement->mDirection.y, p_movement->mForward.y, abs_error);
		EXPECT_NEAR(p_movement->mDirection.z, p_movement->mForward.z, abs_error);

		// Checking movement according to [d = d_0 + v * t] in each direction.
		EXPECT_FLOAT_EQ(p_transform->position.x, previous_position.x + DELTA * p_movement->mMaxVelocity * p_movement->mDirection.x);
		EXPECT_FLOAT_EQ(p_transform->position.y, previous_position.y + DELTA * p_movement->mMaxVelocity * p_movement->mDirection.y);
		EXPECT_FLOAT_EQ(p_transform->position.z, previous_position.z + DELTA * p_movement->mMaxVelocity * p_movement->mDirection.z);

		previous_position = p_transform->position;

		// BACKWARD
		ecs.update(DELTA);

		// Checking direction.
		EXPECT_NEAR(p_movement->mDirection.x, -p_movement->mForward.x, abs_error);
		EXPECT_NEAR(p_movement->mDirection.y, p_movement->mForward.y, abs_error);
		EXPECT_NEAR(p_movement->mDirection.z, p_movement->mForward.z, abs_error);

		// Checking movement according to [d = d_0 + v * t] in each direction.
		EXPECT_FLOAT_EQ(p_transform->position.x, previous_position.x + DELTA * p_movement->mMaxVelocity * p_movement->mDirection.x);
		EXPECT_FLOAT_EQ(p_transform->position.y, previous_position.y + DELTA * p_movement->mMaxVelocity * p_movement->mDirection.y);
		EXPECT_FLOAT_EQ(p_transform->position.z, previous_position.z + DELTA * p_movement->mMaxVelocity * p_movement->mDirection.z);

		previous_position = p_transform->position;

		// RIGHT
		ecs.update(DELTA);

		// Checking direction.
		EXPECT_NEAR(p_movement->mDirection.x, p_movement->mForward.x - 1.0f, abs_error);
		EXPECT_NEAR(p_movement->mDirection.y, p_movement->mForward.y, abs_error);
		EXPECT_NEAR(p_movement->mDirection.z, p_movement->mForward.z + 1.0f, abs_error);

		// Checking movement according to [d = d_0 + v * t] in each direction.
		EXPECT_FLOAT_EQ(p_transform->position.x, previous_position.x + DELTA * p_movement->mMaxVelocity * p_movement->mDirection.x);
		EXPECT_FLOAT_EQ(p_transform->position.y, previous_position.y + DELTA * p_movement->mMaxVelocity * p_movement->mDirection.y);
		EXPECT_FLOAT_EQ(p_transform->position.z, previous_position.z + DELTA * p_movement->mMaxVelocity * p_movement->mDirection.z);
		
		previous_position = p_transform->position;

		// LEFT
		ecs.update(DELTA);

		// Checking direction.
		EXPECT_NEAR(p_movement->mDirection.x, p_movement->mForward.x - 1.0f, abs_error);
		EXPECT_NEAR(p_movement->mDirection.y, p_movement->mForward.y, abs_error);
		EXPECT_NEAR(p_movement->mDirection.z, p_movement->mForward.z - 1.0f, abs_error);

		// Checking movement according to [d = d_0 + v * t] in each direction.
		EXPECT_FLOAT_EQ(p_transform->position.x, previous_position.x + DELTA * p_movement->mMaxVelocity * p_movement->mDirection.x);
		EXPECT_FLOAT_EQ(p_transform->position.y, previous_position.y + DELTA * p_movement->mMaxVelocity * p_movement->mDirection.y);
		EXPECT_FLOAT_EQ(p_transform->position.z, previous_position.z + DELTA * p_movement->mMaxVelocity * p_movement->mDirection.z);

	}

} // MovementLogic
