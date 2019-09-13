#include "pch.h"
TEST(TestCaseName, TestName) {
	ecs::EntityComponentSystem e;
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}