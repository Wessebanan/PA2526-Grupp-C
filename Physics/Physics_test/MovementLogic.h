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

		MovementInputEvent eve;

		if (MovementInputEvent::typeID == eve.getTypeID())
		{
			int a = 0;
		}
		// Temporary system to see if movement systems handle inputs correctly.
		ecs.createSystem<ecs::systems::InputSystem>(0);

		ecs.createSystem<ecs::systems::StaticMovementSystem>(1);
		ecs.createSystem<ecs::systems::StaticMovementUpdateSystem>(1);
		
		MovementComponent movement;
		TransformComponent transform;

		ecs::Entity* movable_entity = ecs.createEntity(movement, transform);

		for (int i = 0; i < 40; i++)
		{
			ecs.update(0.1f);
			
			MovementComponent* p_movement = dynamic_cast<MovementComponent*>(ecs.getComponent(MovementComponent::typeID, movable_entity->getComponentID(MovementComponent::typeID)));
			std::cout << p_movement->mVelocity;
		}
	}
} // MovementLogic
