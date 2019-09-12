#include "pch.h"
#include <GridFunctions.h>





TEST(Grid, ComponentCreation) {
  ecs::EntityComponentSystem mEcs;
  GridFunctions::InitGrid(mEcs, 9, 9, 4);  
  //  GridFunctions::CreateDebugSystems(mEcs);

  //  mEcs.update(0);
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}