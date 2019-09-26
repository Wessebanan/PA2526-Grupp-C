#include "pch.h"
#include "GridFunctions.h"
#include "AISystems.h"

TEST(GridFunctions, InitGrid) {

	int nr_of_rows = 50;
	int nr_of_columns = 50;
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
	size_t number_of_tile_components = mEcs.getComponentCountOfType(ecs::components::TileComponent::typeID);
	size_t number_of_transform_componenets = mEcs.getComponentCountOfType(ecs::components::TransformComponent::typeID);

	//Check so that the correct number of components was created.
    EXPECT_EQ(number_of_transform_componenets + number_of_tile_components, count * 2);
	EXPECT_EQ(number_of_transform_componenets, count);
	EXPECT_EQ(number_of_tile_components, count);
} 

TEST(GridFunctions, CreateDebugSystems) {
	ecs::EntityComponentSystem mEcs;
	GridFunctions::CreateDebugSystems(mEcs);	

	size_t number_of_systems = mEcs.getTotalSystemCount();

	//Check so that the debug system was created.
	EXPECT_EQ(number_of_systems, 1);
}

TEST(AI, CreateAIComponents) {
	ecs::EntityComponentSystem my_ecs;
	ecs::components::PathfindingStateComponent psc;
	ecs::components::MoveStateComponent msc;
	ecs::components::IdleStateComponent isc;

	my_ecs.createEntity(psc, msc, isc);
	size_t number_of_components = my_ecs.getTotalComponentCount();

	//Check so that the debug system was created.
	EXPECT_EQ(number_of_components, 3);
}

TEST(AI, CreateAISystems) {
	ecs::EntityComponentSystem my_ecs;
	my_ecs.createSystem<ecs::systems::PathfindingStateSystem>();
	my_ecs.createSystem<ecs::systems::IdleStateSystem>();
	my_ecs.createSystem<ecs::systems::MoveStateSystem>();

	size_t number_of_systems = my_ecs.getTotalSystemCount();

	//Check so that the debug system was created.
	EXPECT_EQ(number_of_systems, 3);
}