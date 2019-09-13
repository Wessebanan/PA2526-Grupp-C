#include "pch.h"
#include "ecs.h"

TEST(TestCaseName, TestName) {
	ecs::EntityComponentSystem e;
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}
TEST(TestCaseName, Teeee)
{
	EXPECT_EQ(1, 2);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}