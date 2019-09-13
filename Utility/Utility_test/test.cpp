#include "pch.h"
#include "CameraFunctions.h"
#include "UtilityFunctions.h"

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(TestCaseName, TestName) {
  ecs::EntityComponentSystem mEcs;
  CameraFunctions::InitDevCamera(mEcs);
  CameraFunctions::InitCameraSystems(mEcs);
	
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}