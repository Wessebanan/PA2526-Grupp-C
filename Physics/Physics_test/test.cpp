#include "pch.h"
#include "MovementSystem.h"
#include "Fbx_Loader.h"
#include "CameraSystems.h"
#include "DebugInfo.h"

TEST(TestCaseName, TestName) {
	ecs::EntityComponentSystem e;
	e.createSystem<ecs::systems::StaticMovementSystem>();

	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}