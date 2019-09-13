#include "pch.h"
#include "InitInputHandler.h"

TEST(ECS, testInitECS)
{
	ecs::EntityComponentSystem myECS;

	initInputECS(myECS);
	EXPECT_TRUE(true);
}
