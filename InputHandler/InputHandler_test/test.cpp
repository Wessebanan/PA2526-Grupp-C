#include "pch.h"
#include "..//InputHandler/includes/InitInputHandler.h"

TEST(TestCaseName, TestName) {
	int a = 0;
	
	ecs::EntityComponentSystem rECS;



	initInputECS(rECS);

  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}