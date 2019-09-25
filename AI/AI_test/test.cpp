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
	GridFunctions::CreateGrid(mEcs, nr_of_rows, nr_of_columns, radius);
	ecs::ComponentIterator it = mEcs.getAllComponentsOfType(ecs::components::TransformComponent::typeID);
	ecs::BaseComponent *p_base;
	ecs::components::TransformComponent *p_transform;
	while (p_base = it.next())
	{
		p_transform = (ecs::components::TransformComponent*)p_base;
		if (p_transform->position.y == 3.f)
		{
			found = true;
		}
	}
	
	EXPECT_EQ(found, true);
}

