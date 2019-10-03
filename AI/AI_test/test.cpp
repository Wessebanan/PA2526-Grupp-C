#include "pch.h"
#include "AIGlobals.h"
#include "GridProp.h"
#include "GridFunctions.h"
#include "AIFunctions.h"
#include <iostream>

TEST(GridFunctions, InitGrid) {

	int nr_of_rows = ARENA_ROWS;
	int nr_of_columns = ARENA_COLUMNS;
	float radius = TILE_RADIUS;
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
	ecs::EntityComponentSystem my_ecs;
	my_ecs.initialize(desc);


	//Call function that will create the components of the grid.
	GridFunctions::CreateGrid(my_ecs, nr_of_rows, nr_of_columns, radius);
	//Fetch the number of components created of each type.
	int number_of_tile_components = my_ecs.getComponentCountOfType(ecs::components::TileComponent::typeID);
	int number_of_transform_componenets = my_ecs.getComponentCountOfType(ecs::components::TransformComponent::typeID);

	//Check so that the correct number of components was created.
    EXPECT_EQ(number_of_transform_componenets + number_of_tile_components, count * 2);
	EXPECT_EQ(number_of_transform_componenets, count);
	EXPECT_EQ(number_of_tile_components, count);
} 

TEST(GridFunctions, CreateDebugSystems) {
	ecs::EntityComponentSystem my_ecs;
	GridFunctions::CreateDebugSystems(my_ecs);

	int number_of_systems = my_ecs.getTotalSystemCount();

	//Check so that the debug system was created.
	EXPECT_EQ(number_of_systems, 1);
}

TEST(GridFunctions, heightMapTest) {
	int nr_of_rows = ARENA_ROWS;
	int nr_of_columns = ARENA_COLUMNS;
	float radius = TILE_RADIUS;
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
	ecs::EntityComponentSystem my_ecs;
	my_ecs.initialize(desc);

	bool found = false;
	int nr_of_components = 0;
	GridFunctions::CreateGrid(my_ecs, nr_of_rows, nr_of_columns, radius); //Create grid with diffrent height
	ecs::ComponentIterator it = my_ecs.getAllComponentsOfType(ecs::components::TransformComponent::typeID); //iterator for all transform components
	ecs::BaseComponent *p_base;
	ecs::components::TransformComponent *p_transform;
	while (p_base = it.next()) //loop through all components and returns a base component
	{
		p_transform = (ecs::components::TransformComponent*)p_base; //casts base component to transform component
		if (p_transform->position.y == 3.f) // check if there is a component with a y pos that is 3(there is only one that has 3 according to the height map)
		{
			found = true;
			nr_of_components++;
		}
	}
	
	EXPECT_EQ(found, true);
	EXPECT_EQ(nr_of_components, 1);
}

TEST(GridFunctions, differentTypes) {
	int nr_of_rows = ARENA_ROWS;
	int nr_of_columns = ARENA_COLUMNS;
	float radius = TILE_RADIUS;
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
	ecs::EntityComponentSystem my_ecs;
	my_ecs.initialize(desc);

	bool stone_found = false;
	bool water_found = false;
	int nr_of_stone = 0;
	int nr_of_water = 0;
	int nr_of_components = 0;

	GridFunctions::CreateGrid(my_ecs, nr_of_rows, nr_of_columns, radius); //Create grid with diffrent height
	ecs::ComponentIterator it = my_ecs.getAllComponentsOfType(ecs::components::TileComponent::typeID); //iterator for all transform components
	ecs::BaseComponent* p_base;
	ecs::components::TileComponent* p_tile;
	while (p_base = it.next()) //loop through all components and returns a base component
	{
		nr_of_components++;
		p_tile = (ecs::components::TileComponent*)p_base; //casts base component to transform component
		if (p_tile->tileType == TileTypes::STONE) // check if there is a component with a stone type and how many stone tiles there are
		{
			stone_found = true;
			nr_of_stone++;
		}
		else if (p_tile->tileType == TileTypes::WATER)// check if there is a component with a water type and how many water tiles there are
		{
			water_found = true;
			nr_of_water++;
		}
	}

	EXPECT_EQ(stone_found, true);
	EXPECT_EQ(water_found, true);
	EXPECT_EQ(nr_of_stone, 1); //the map is predifined so expectation for how many tile of a type can be tested 
	EXPECT_EQ(nr_of_water, 10);
}

TEST(AI, CreateComponents) {
	ecs::EntityComponentSystem my_ecs;
	ecs::components::PathfindingStateComponent psc;
	ecs::components::IdleStateComponent isc;
	ecs::components::MoveStateComponent msc;
	ecs::components::UnitComponent uc;
	ecs::components::ArmyComponent ac;
		
	my_ecs.createEntity(psc, isc, msc);
	my_ecs.createEntity(uc, ac);

	int number_of_components = my_ecs.getTotalComponentCount();

	//Check so that the debug system was created.
	EXPECT_EQ(number_of_components, 5);
}

TEST(AI, CreateSystems) {
	ecs::EntityComponentSystem my_ecs;
	my_ecs.createSystem<ecs::systems::PathfindingStateSystem>();
	my_ecs.createSystem<ecs::systems::IdleStateSystem>();
	my_ecs.createSystem<ecs::systems::MoveStateSystem>();
	my_ecs.createSystem<ecs::systems::SwitchStateSystem>();

	int number_of_systems = my_ecs.getTotalSystemCount();

	//Check so that the debug system was created.
	EXPECT_EQ(number_of_systems, 4);
}

TEST(AIFunctions, CreatePlayerArmies) {
	int nr_of_rows = ARENA_ROWS;
	int nr_of_columns = ARENA_COLUMNS;
	float radius = TILE_RADIUS;
	unsigned int count = nr_of_columns * nr_of_rows * 2;
	ecs::CompTypeMemDesc types[] = {
		{ TileComponent::typeID, TileComponent::size, count},
		{ TransformComponent::typeID, TransformComponent::size, count},
	};
	ecs::ECSDesc desc;
	desc.compTypeCount = 2;
	desc.compTypeMemDescs = types;
	desc.systemLayerCount = 10;
	ecs::EntityComponentSystem my_ecs;
	my_ecs.initialize(desc);

	//Create the grid so that we can find the starting positions for the army.
	GridFunctions::CreateGrid(my_ecs, nr_of_rows, nr_of_columns, radius);
	//Create the user entities and all of their unit entities.
	AIFunctions::CreatePlayerArmies(my_ecs);

	int expected_number_of_components = 4 * PlayerProperties::numberOfUnits * 3 + 4 + 12*12*2;
	size_t number_of_components = my_ecs.getTotalComponentCount();
	
	//Check so that the debug system was created.
	EXPECT_EQ(number_of_components, expected_number_of_components);
}

TEST(AIFunctions, SwitchStatesOfArmy) {
	int nr_of_rows = ARENA_ROWS;
	int nr_of_columns = ARENA_COLUMNS;
	float radius = TILE_RADIUS;
	unsigned int count = nr_of_columns * nr_of_rows * 2;
	ecs::CompTypeMemDesc types[] = {
		{ TileComponent::typeID, TileComponent::size, count},
		{ TransformComponent::typeID, TransformComponent::size, count},
	};
	ecs::ECSDesc desc;
	desc.compTypeCount = 2;
	desc.compTypeMemDescs = types;
	desc.systemLayerCount = 10;
	ecs::EntityComponentSystem my_ecs;
	my_ecs.initialize(desc);

	//Create the grid so that we can find the starting positions for the army.
	GridFunctions::CreateGrid(my_ecs, nr_of_rows, nr_of_columns, radius);
	//Create the user entities and all of their unit entities.
	AIFunctions::CreatePlayerArmies(my_ecs);
	my_ecs.createSystem<ecs::systems::SwitchStateSystem>();

	int expected_number_of_idle_components = 12;
	size_t number_of_idle_components = my_ecs.getComponentCountOfType(ecs::components::IdleStateComponent::typeID);
	//Check so that the debug system was created.
	EXPECT_EQ(number_of_idle_components, expected_number_of_idle_components);
	int expected_number_of_move_components = 0;
	size_t number_of_move_components = my_ecs.getComponentCountOfType(ecs::components::MoveStateComponent::typeID);
	//Check the number of move components.
	EXPECT_EQ(number_of_move_components, expected_number_of_move_components);

	//Create an event and update the ecs
	ecs::events::ChangeUserStateEvent test_event;
	test_event.playerId = PLAYER::PLAYER1;
	test_event.newState = STATE::MOVE;
	my_ecs.createEvent(test_event);
	my_ecs.update(0.1f);

	expected_number_of_idle_components = 9;
	number_of_idle_components = my_ecs.getComponentCountOfType(ecs::components::IdleStateComponent::typeID);
	//Check the number of idle components.
	EXPECT_EQ(number_of_idle_components, expected_number_of_idle_components);
	expected_number_of_move_components = 3;
	number_of_move_components = my_ecs.getComponentCountOfType(ecs::components::MoveStateComponent::typeID);
	//Check the number of move components.
	EXPECT_EQ(number_of_move_components, expected_number_of_move_components);
}

TEST(PotentialField, CreatePotentialField)
{
	int nr_of_rows = ARENA_ROWS;
	int nr_of_columns = ARENA_COLUMNS;
	float radius = TILE_RADIUS;
	unsigned int count = nr_of_columns * nr_of_rows;
	int nr_of_nice = 0;
	int iterr = 0;

	//Define some ECS stuff to allow the ECS to create more than 100 of each component.
	ecs::CompTypeMemDesc types[] = {
		{ TileComponent::typeID, TileComponent::size, count},
		{ TransformComponent::typeID, TransformComponent::size, count},
	};
	ecs::ECSDesc desc;
	desc.compTypeCount = 2;
	desc.compTypeMemDescs = types;
	desc.systemLayerCount = 10;
	ecs::EntityComponentSystem my_ecs;
	my_ecs.initialize(desc);
	GridFunctions::CreateGrid(my_ecs, nr_of_rows, nr_of_columns, radius); //Create grid with potential field

	ecs::ComponentIterator it = my_ecs.getAllComponentsOfType(ecs::components::TileComponent::typeID); //iterator for all transform components
	ecs::BaseComponent* p_base;
	ecs::components::TileComponent* p_tile;
	while (p_base = it.next()) //loop through all components and returns a base component
	{
		p_tile = (ecs::components::TileComponent*)p_base; //casts base component to tile component
		std::cout << p_tile->niceness << " "; //print all charges
		iterr++;
		if (iterr % 12 == 0)
			std::cout << endl;
		if (p_tile->niceness == -5) 
		{
			nr_of_nice++;
		}
	}
	EXPECT_EQ(nr_of_nice, 10); // test if there are 10 charges with -5 niceness as the predefined map is designed
}
