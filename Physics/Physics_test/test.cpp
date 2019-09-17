#include "pch.h"
#include "MovementSystem.h"

TEST(TestCaseName, TestName) {
	ecs::EntityComponentSystem e;
	e.createSystem<StaticMovementSystem>();

	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}