#include "pch.h"
#include <GridFunctions.h>

TEST(GridFunctions, InitGrid) {

	int nr_of_rows = 12;
	int nr_of_columns = 12;
	float radius = 4.0f;
	unsigned int count = nr_of_columns * nr_of_rows;
	//Define some ECS stuff to allow the ECS to create more than 100 of each component.
	ecs::CompTypeMemDesc types[] = {
		{ TileComponent::typeID, TileComponent::size, count},
		{ TransformComponent::typeID, TransformComponent::size, count},
	};
	ecs::ECSDesc desc;
	desc.compTypeCount = 2;
	desc.compTypeMemDescs = types;
	desc.systemLayerCount = 10;
	ecs::EntityComponentSystem mEcs;
	mEcs.initialize(desc);


	//Call function that will create the components of the grid.
	GridFunctions::CreateGrid(mEcs, nr_of_rows, nr_of_columns, radius);
	//Fetch the number of components created of each type.
	int number_of_tile_components = mEcs.getComponentCountOfType(ecs::components::TileComponent::typeID);
	int number_of_transform_componenets = mEcs.getComponentCountOfType(ecs::components::TransformComponent::typeID);

	//Check so that the correct number of components was created.
    EXPECT_EQ(number_of_transform_componenets + number_of_tile_components, count * 2);
	EXPECT_EQ(number_of_transform_componenets, count);
	EXPECT_EQ(number_of_tile_components, count);
} 

TEST(GridFunctions, CreateDebugSystems) {
	ecs::EntityComponentSystem mEcs;
	GridFunctions::CreateDebugSystems(mEcs);	

	int number_of_systems = mEcs.getTotalSystemCount();

	//Check so that the debug system was created.
	EXPECT_EQ(number_of_systems, 1);
}

TEST(GridFunctions, heightMapTest) {
	int nr_of_rows = 12;
	int nr_of_columns = 12;
	float radius = 4.0f;
	unsigned int count = nr_of_columns * nr_of_rows;
	//Define some ECS stuff to allow the ECS to create more than 100 of each component.
	ecs::CompTypeMemDesc types[] = {
		{ TileComponent::typeID, TileComponent::size, count},
		{ TransformComponent::typeID, TransformComponent::size, count},
	};
	ecs::ECSDesc desc;
	desc.compTypeCount = 2;
	desc.compTypeMemDescs = types;
	desc.systemLayerCount = 10;
	ecs::EntityComponentSystem mEcs;
	mEcs.initialize(desc);

	bool found = false;
	int nrOfComponents = 0;
	GridFunctions::CreateGrid(mEcs, nr_of_rows, nr_of_columns, radius); //Create grid with diffrent height
	ecs::ComponentIterator it = mEcs.getAllComponentsOfType(ecs::components::TransformComponent::typeID); //iterator for all transform components
	ecs::BaseComponent *p_base;
	ecs::components::TransformComponent *p_transform;
	while (p_base = it.next()) //loop through all components and returns a base component
	{
		p_transform = (ecs::components::TransformComponent*)p_base; //casts base component to transform component
		if (p_transform->position.y == 3.f) // check if there is a component with a y pos that is 3(there is only one that has 3 according to the height map)
		{
			found = true;
			nrOfComponents++;
		}
	}
	
	EXPECT_EQ(found, true);
	EXPECT_EQ(nrOfComponents, 1);
}

TEST(GridFunctions, differentTypes) {
	int nr_of_rows = 12;
	int nr_of_columns = 12;
	float radius = 4.0f;
	unsigned int count = nr_of_columns * nr_of_rows;

	//Define some ECS stuff to allow the ECS to create more than 100 of each component.
	ecs::CompTypeMemDesc types[] = {
		{ TileComponent::typeID, TileComponent::size, count},
		{ TransformComponent::typeID, TransformComponent::size, count},
	};
	ecs::ECSDesc desc;
	desc.compTypeCount = 2;
	desc.compTypeMemDescs = types;
	desc.systemLayerCount = 10;
	ecs::EntityComponentSystem mEcs;
	mEcs.initialize(desc);

	bool stone_found = false;
	bool water_found = false;
	int nr_of_stone = 0;
	int nr_of_water = 0;
	int nr_of_components = 0;

	GridFunctions::CreateGrid(mEcs, nr_of_rows, nr_of_columns, radius); //Create grid with diffrent height
	ecs::ComponentIterator it = mEcs.getAllComponentsOfType(ecs::components::TileComponent::typeID); //iterator for all transform components
	ecs::BaseComponent* p_base;
	ecs::components::TileComponent* p_tile;
	while (p_base = it.next()) //loop through all components and returns a base component
	{
		nr_of_components++;
		p_tile = (ecs::components::TileComponent*)p_base; //casts base component to transform component
		if (p_tile->tileType == TileTypes::Stone) // check if there is a component with a stone type and how many stone tiles there are
		{
			stone_found = true;
			nr_of_stone++;
		}
		else if (p_tile->tileType == TileTypes::Water)// check if there is a component with a water type and how many water tiles there are
		{
			water_found = true;
			nr_of_water++;
		}
	}

	EXPECT_EQ(stone_found, true);
	EXPECT_EQ(water_found, true);
	EXPECT_EQ(nr_of_stone, 1); 
	EXPECT_EQ(nr_of_water, 10);
}

