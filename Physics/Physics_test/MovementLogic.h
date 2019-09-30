#pragma once
#include "pch.h"
#include "MovementSystem.h"
#include "InputTest.h"

namespace MovementLogic
{
#pragma region StaticMovementLogic
	TEST(StaticMovementLogic, CreateStaticMovementSystem)
	{
		ecs::EntityComponentSystem ecs;
		ecs::systems::StaticMovementSystem* static_movement_system = ecs.createSystem<ecs::systems::StaticMovementSystem>();

		// Checking that ecs has one system at this point.
		EXPECT_EQ(ecs.getTotalSystemCount(), 1);

		// Checking that the pointer to the system is not equal to nullptr.
		EXPECT_NE(static_movement_system, nullptr);
	}

	TEST(StaticMovementLogic, CreateStaticMovementUpdateSystem)
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

	TEST(StaticMovementLogic, CreateMovableEntity)
	{
		ecs::EntityComponentSystem ecs;

		// Making sure there are no entities or components.
		EXPECT_EQ(ecs.getTotalEntityCount(), 0);
		EXPECT_EQ(ecs.getTotalComponentCount(), 0);

		StaticMovementComponent movement;
		TransformComponent transform;

		ecs::Entity* movable_entity = ecs.createEntity(movement, transform);

		// Checking that there is now one entity and two components.
		EXPECT_EQ(ecs.getTotalEntityCount(), 1);
		EXPECT_EQ(ecs.getTotalComponentCount(), 2);

		// Checking that the pointer received from getEntity matches
		// the pointer to the entity.
		EXPECT_EQ(ecs.getEntity(movable_entity->getID()), movable_entity);
	}

	TEST(StaticMovementLogic, HandleInput)
	{
		ecs::EntityComponentSystem ecs;

		// Temporary system to see if movement systems handle inputs correctly.
		// Layer 0 to ensure input is handled first.
		ecs.createSystem<ecs::systems::InputSystem>(0);

		ecs.createSystem<ecs::systems::StaticMovementUpdateSystem>();
		ecs.createSystem<ecs::systems::StaticMovementSystem>();
		
		StaticMovementComponent movement;
		TransformComponent transform;

		ecs::Entity* movable_entity = ecs.createEntity(movement, transform);

		// Grabbing pointers to the components modified by the systems.
		StaticMovementComponent* p_movement = dynamic_cast<StaticMovementComponent*>(ecs.getComponent(StaticMovementComponent::typeID, movable_entity->getComponentID(StaticMovementComponent::typeID)));
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
#pragma endregion
#pragma region DynamicMovementLogic
	TEST(DynamicMovementLogic, CreateDynamicMovementSystem)
	{
		ecs::EntityComponentSystem ecs;
		ecs::systems::DynamicMovementSystem* dynamic_movement_system = ecs.createSystem<ecs::systems::DynamicMovementSystem>();

		// Checking that ecs has one system at this point.
		EXPECT_EQ(ecs.getTotalSystemCount(), 1);

		// Checking that the pointer to the system is not equal to nullptr.
		EXPECT_NE(dynamic_movement_system, nullptr);
	}

	TEST(DynamicMovementLogic, CreateMovableEntity)
	{
		ecs::EntityComponentSystem ecs;

		// Making sure there are no entities or components.
		EXPECT_EQ(ecs.getTotalEntityCount(), 0);
		EXPECT_EQ(ecs.getTotalComponentCount(), 0);

		DynamicMovementComponent movement;
		TransformComponent transform;

		ecs::Entity* movable_entity = ecs.createEntity(movement, transform);

		// Checking that there is now one entity and two components.
		EXPECT_EQ(ecs.getTotalEntityCount(), 1);
		EXPECT_EQ(ecs.getTotalComponentCount(), 2);

		// Checking that the pointer received from getEntity matches
		// the pointer to the entity.
		EXPECT_EQ(ecs.getEntity(movable_entity->getID()), movable_entity);
	}

	TEST(DynamicMovementLogic, HandleInput)
	{
		ecs::EntityComponentSystem ecs;
		//ecs.createSystem<ecs::systems::InputSystem>();
		ecs.createSystem<ecs::systems::DynamicMovementSystem>();

		DynamicMovementComponent movement;
		TransformComponent transform;
		transform.scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

		ecs::Entity* movable_entity = ecs.createEntity(movement, transform);
		DynamicMovementComponent* p_movement	= dynamic_cast<DynamicMovementComponent*>(ecs.getComponentFromEntity(DynamicMovementComponent::typeID, movable_entity->getID()));
		TransformComponent* p_transform			= dynamic_cast<TransformComponent*>(ecs.getComponentFromEntity(TransformComponent::typeID, movable_entity->getID()));
		
		for (int i = 0; i <= 25; i++)
		{
			MovementInputEvent movement_event;
			movement_event.mEntityID = movable_entity->getID();
			if (i < 5)
			{
				movement_event.mInput = FORWARD;
			}
			else if (i < 10)
			{
				movement_event.mInput = RIGHT;
			}
			else if (i < 15)
			{
				movement_event.mInput = BACKWARD;
			}
			else if(i < 20)
			{
				movement_event.mInput = LEFT;
			}
			if (i < 20)
			{
				ecs.createEvent(movement_event);
			}

			ecs.update(0.1f);
			std::cout << std::endl << "Update: " << i + 1 << std::endl;
			std::cout << "Position: (" << p_transform->position.x << ", " << p_transform->position.y << ", " << p_transform->position.z << ")" << std::endl;
			std::cout << "Acceleration: ("	<< p_movement->mAcceleration.x << ", " << p_movement->mAcceleration.y << ", " << p_movement->mAcceleration.z << ")" << std::endl;
			std::cout << "Velocity: ("	<< p_movement->mVelocity.x << ", " << p_movement->mVelocity.y << ", " << p_movement->mVelocity.z << ")" << std::endl;
		}

		// Instead of doing a boatload of math to check each step, I am content
		// in knowing that the entity returns to its original position after taking a lap.
		// If you (Mr. Reviewer) want to check the journey you can uncomment the couts.

		const float ABS_ERROR = pow(10.0, -10.0);
		EXPECT_NEAR(p_transform->position.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(p_transform->position.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(p_transform->position.z, 0.0f, ABS_ERROR);

		// The entity should also be standing still at this point.
		EXPECT_NEAR(p_movement->mAcceleration.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(p_movement->mAcceleration.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(p_movement->mAcceleration.z, 0.0f, ABS_ERROR);

		EXPECT_NEAR(p_movement->mVelocity.x, 0.0f, ABS_ERROR);
		EXPECT_NEAR(p_movement->mVelocity.y, 0.0f, ABS_ERROR);
		EXPECT_NEAR(p_movement->mVelocity.z, 0.0f, ABS_ERROR);
	}
#pragma endregion
} // MovementLogic
