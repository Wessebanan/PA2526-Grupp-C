#pragma once
#include "AIEcsFunctions.h"
#include "GridEcsFunctions.h"


#pragma region AISystemTests
TEST(AISystemTests, CreateStateSystems)
{
	//Create the ecs
	ecs::EntityComponentSystem ecs;
	//Create all of the state systems and the switch state system
	ecs.createSystem<ecs::systems::IdleStateSystem>();
	ecs.createSystem<ecs::systems::PathfindingStateSystem>();
	ecs.createSystem<ecs::systems::MoveStateSystem>();
	ecs.createSystem<ecs::systems::AttackStateSystem>();
	ecs.createSystem<ecs::systems::LootStateSystem>();
	ecs.createSystem<ecs::systems::SwitchStateSystem>();
	//Check that the correct number of systems was acctually created
	int nr_of_systems = ecs.getTotalSystemCount();
	EXPECT_EQ(nr_of_systems, 6);
}

TEST(AISystemTests, TestFightLoop)
{
	//Create the ecs with increased number of Tile,Color and Transform components since we are going to need
	//the grid and the armies for this test.
	ecs::CompTypeMemDesc ecsMemDesc[] = {
			{ TileComponent::typeID, TileComponent::size, 1000 },
			{ ColorComponent::typeID, ColorComponent::size, 1000 },
			{ TransformComponent::typeID, TransformComponent::size, 1000 },
	};

	ecs::ECSDesc ecsDesc;
	ecsDesc.compTypeCount = 3;
	ecsDesc.compTypeMemDescs = ecsMemDesc;
	ecsDesc.systemLayerCount = 10;

	ecs::EntityComponentSystem ecs;
	ecs.initialize(ecsDesc);
	//Create all of the state systems needed in this command loop and the switch state system
	ecs.createSystem<ecs::systems::PathfindingStateSystem>();
	ecs.createSystem<ecs::systems::MoveStateSystem>();
	ecs.createSystem<ecs::systems::AttackStateSystem>();
	ecs.createSystem<ecs::systems::SwitchStateSystem>();
	//Create the grid and the player armies.
	GridEcsFunctions::CreateGrid(ecs, MAX_ARENA_ROWS, MAX_ARENA_COLUMNS, TILE_RADIUS);
	AIEcsFunctions::CreatePlayerArmies(ecs);
	//Create the event and dispatch it in the ecs.
	ecs::events::ChangeUserStateEvent test_event;
	test_event.playerId = PLAYER::PLAYER1;
	test_event.newState = STATE::ATTACK;
	ecs.createEvent(test_event);
	ecs.update(1.0f);
	//Check how many pathfinding components that is inside the system should be 3
	int number_of_path_comps = ecs.getComponentCountOfType(ecs::components::PathfindingStateComponent::typeID);
	EXPECT_EQ(number_of_path_comps, 3);
	ecs.update(0.1f);
	//Check how many move components that is inside the system should be 3
	int number_of_move_comps = ecs.getComponentCountOfType(ecs::components::MoveStateComponent::typeID);
	number_of_path_comps = ecs.getComponentCountOfType(ecs::components::PathfindingStateComponent::typeID);
	EXPECT_EQ(number_of_move_comps, 3);
	ecs.update(0.5f);
	//Check how many pathfinding components that is inside the system should be 0
	number_of_path_comps = ecs.getComponentCountOfType(ecs::components::PathfindingStateComponent::typeID);
	EXPECT_EQ(number_of_path_comps, 0);
	/* NEEDS A FORCED FIGHT STATE TO TEST THE LOOP WHEN THE ATTACK STATE IS FINNISHED */
}

TEST(AISystemTests, TestIdleLoop)
{
	//Create the ecs with increased number of Tile,Color and Transform components since we are going to need
		//the grid and the armies for this test.
	ecs::CompTypeMemDesc ecsMemDesc[] = {
			{ TileComponent::typeID, TileComponent::size, 1000 },
			{ ColorComponent::typeID, ColorComponent::size, 1000 },
			{ TransformComponent::typeID, TransformComponent::size, 1000 },
	};

	ecs::ECSDesc ecsDesc;
	ecsDesc.compTypeCount = 3;
	ecsDesc.compTypeMemDescs = ecsMemDesc;
	ecsDesc.systemLayerCount = 10;

	ecs::EntityComponentSystem ecs;
	ecs.initialize(ecsDesc);
	//Create all of the state systems needed in this command loop and the switch state system
	ecs.createSystem<ecs::systems::AttackStateSystem>();
	ecs.createSystem<ecs::systems::IdleStateSystem>();
	ecs.createSystem<ecs::systems::SwitchStateSystem>();
	//Create the grid and the player armies.
	GridEcsFunctions::CreateGrid(ecs, MAX_ARENA_ROWS, MAX_ARENA_COLUMNS, TILE_RADIUS);
	AIEcsFunctions::CreatePlayerArmies(ecs);
	//Check how many pathfinding components that is inside the system should be 12 since every army starts in idle
	int number_of_idle_comps = ecs.getComponentCountOfType(ecs::components::IdleStateComponent::typeID);
	EXPECT_EQ(number_of_idle_comps, 12);
	ecs.update(0.1f);
	///Check how many pathfinding components that is inside the system should be 12 since every army starts in idle
	number_of_idle_comps = ecs.getComponentCountOfType(ecs::components::IdleStateComponent::typeID);
	EXPECT_EQ(number_of_idle_comps, 12);
	ecs.update(1.1f);
	//Check how many pathfinding components that is inside the system should be 12 since every army starts in idle
	number_of_idle_comps = ecs.getComponentCountOfType(ecs::components::IdleStateComponent::typeID);
	EXPECT_EQ(number_of_idle_comps, 12);
	ecs.update(10.1f);
	//Check how many pathfinding components that is inside the system should be 12 since every army starts in idle
	number_of_idle_comps = ecs.getComponentCountOfType(ecs::components::IdleStateComponent::typeID);
	EXPECT_EQ(number_of_idle_comps, 12);
	ecs.update(3.1f);
}

TEST(AISystemTests, TestLootLoop)
{
	//Create the ecs with increased number of Tile,Color and Transform components since we are going to need
		//the grid and the armies for this test.
	ecs::CompTypeMemDesc ecsMemDesc[] = {
			{ TileComponent::typeID, TileComponent::size, 1000 },
			{ ColorComponent::typeID, ColorComponent::size, 1000 },
			{ TransformComponent::typeID, TransformComponent::size, 1000 },
	};

	ecs::ECSDesc ecsDesc;
	ecsDesc.compTypeCount = 3;
	ecsDesc.compTypeMemDescs = ecsMemDesc;
	ecsDesc.systemLayerCount = 10;

	ecs::EntityComponentSystem ecs;
	ecs.initialize(ecsDesc);
	//Create all of the state systems needed in this command loop and the switch state system
	ecs.createSystem<ecs::systems::PathfindingStateSystem>();
	ecs.createSystem<ecs::systems::IdleStateSystem>();
	ecs.createSystem<ecs::systems::MoveStateSystem>();
	ecs.createSystem<ecs::systems::LootStateSystem>();
	ecs.createSystem<ecs::systems::SwitchStateSystem>();

	//Create the grid and the player armies.
	GridEcsFunctions::CreateGrid(ecs, MAX_ARENA_ROWS, MAX_ARENA_COLUMNS, TILE_RADIUS);
	AIEcsFunctions::CreatePlayerArmies(ecs);
	//Create the event and dispatch it in the ecs.
	ecs::events::ChangeUserStateEvent test_event;
	test_event.playerId = PLAYER::PLAYER1;
	test_event.newState = STATE::LOOT;
	ecs.createEvent(test_event);
	ecs.update(1.0f);
	//Check how many PathfindingStateComponents there is in the system, should be 0 since the PathSystem runs after t
	int number_of_path_comps = ecs.getComponentCountOfType(ecs::components::PathfindingStateComponent::typeID);
	EXPECT_EQ(number_of_path_comps, 3);
	ecs.update(0.1f);
	//Check how many LootStateComponents there is in the system, should be 3
	int number_of_move_comps = ecs.getComponentCountOfType(ecs::components::MoveStateComponent::typeID);
	EXPECT_EQ(number_of_move_comps, 3);

	/* WILL HAVE TO EXPAND THIS TEST WHEN LOOTING SPOTS HAVE BEEN ADDED TO THE GAME */
}
#pragma endregion