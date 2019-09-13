#include "pch.h"
#include <GridFunctions.h>

TEST(GridFunctions, InitGrid) {
  
  ecs::EntityComponentSystem mEcs;
  GridFunctions::InitGrid(mEcs, 50, 50, 4);  
  GridFunctions::CreateDebugSystems(mEcs);

  mEcs.update(0);

  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}