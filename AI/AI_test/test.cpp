#include "pch.h"
#include <GridFunctions.h>

TEST(GridFunctions, InitGrid) {

	unsigned int count = 50 * 50;

	ecs::CompTypeMemDesc types[] = {
		{ TileComponent::typeID, TileComponent::size, count},
		{ PositionComponent::typeID, PositionComponent::size, count},
	};
	ecs::ECSDesc desc;
	desc.compTypeCount = 2;
	desc.compTypeMemDescs = types;
	desc.systemLayerCount = 10;
  
	ecs::EntityComponentSystem mEcs;
	mEcs.initialize(desc);

    GridFunctions::InitGrid(mEcs, 50, 50, 4);  
 
	int number_of_tile_components = mEcs.getComponentCountOfType(ecs::components::TileComponent::typeID);
	int number_of_position_componenets = mEcs.getComponentCountOfType(ecs::components::PositionComponent::typeID);

    EXPECT_EQ(number_of_position_componenets + number_of_tile_components, count * 2);
	EXPECT_EQ(number_of_position_componenets, count);
	EXPECT_EQ(number_of_tile_components, count);
} 

TEST(GridFunctions, CreateDebugSystems) {
	ecs::EntityComponentSystem mEcs;
	GridFunctions::CreateDebugSystems(mEcs);	

	int number_of_systems = mEcs.getTotalSystemCount();

	EXPECT_EQ(number_of_systems, 1);
}

