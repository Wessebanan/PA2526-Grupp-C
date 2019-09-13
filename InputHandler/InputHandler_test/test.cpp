#include "pch.h"
#include "InitInputHandler.h"

TEST(ECS, testInitECS)
{
	ecs::EntityComponentSystem myECS;

	initInputECS(myECS);

	myECS.update(0.2f);

	EXPECT_TRUE(true);
}
