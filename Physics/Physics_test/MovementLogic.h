#pragma once
#include "pch.h"
#include "MovementSystem.h"

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


} // MovementLogic
