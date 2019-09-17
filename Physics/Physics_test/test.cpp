#include "pch.h"
#include "MovementSystem.h"

TEST(TestCaseName, TestName) {
	ecs::EntityComponentSystem ecs;

	MovementComponent movement;
	TransformComponent transform;
	
	ecs.createSystem<ecs::systems::StaticMovementSystem>();
	ecs.createSystem<ecs::systems::StaticMovementUpdateSystem>();

	// Checking to see that both systems are created.
	int system_count = ecs.getTotalSystemCount();
	ASSERT_EQ(system_count, 2);

	
	ecs::Entity* entity = ecs.createEntity(movement, transform);

	

	ecs.update(0.1f);	
	

	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}