#include "pch.h"
#include "InitInputHandler.h"


ecs::EntityComponentSystem myECS;

TEST(TestInputs, Init)
{

	initInputECS(myECS);


}

TEST(TestInput, Update)
{
	myECS.update(1.0f);
}

TEST(TestInput, Keyboard)
{

}


TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}