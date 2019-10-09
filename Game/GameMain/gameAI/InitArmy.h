#pragma once

#include "..//gameAI/AIComponents.h"
#include "..//gameAI/AIFunctions.h"

#include "..//gameUtility/UtilityComponents.h"
#include "ecs.h"

//int2 FindStartingTile(PLAYER Id);

void InitArmy(ecs::EntityComponentSystem& rECS)
{
	AIFunctions::CreatePlayerArmies(rECS);

	////Create Components for a "User" entity.
	//ecs::components::ArmyComponent army;
	////Create Components for a "Unit" entity.
	//ecs::components::TransformComponent transform;
	//ecs::components::UnitComponent unit;
	//ecs::components::IdleStateComponent idle_state;
	////Temporary entity pointer so that we can fetch the units IDs so that we can store
	////them in the army component.
	//ecs::Entity* temp_entity;
	//int2 starting_tile_index;
	//ID temp_id;
	//ecs::components::TransformComponent* p_transform;
	//GridProp* p_gp = GridProp::GetInstance();
	////Loop for every player.
	//for (int i = 0; i < 4; i++)
	//{
	//	////Fetch the index of the starting tile for this player.
	//	starting_tile_index = FindStartingTile((PLAYER)i);
	//	temp_id = p_gp->mGrid[starting_tile_index.y][starting_tile_index.x].Id;
	//	p_transform = rECS.getComponentFromEntity<ecs::components::TransformComponent>(temp_id);
	//	//Set current players enum ID for this armies units.
	//	unit.playerID = (PLAYER)i;

	//	for (int u = 0; u < PlayerProperties::numberOfUnits; u++)
	//	{
	//		//Set starting position of the unit.
	//		transform.position.x = p_transform->position.x + u * 10.0f;
	//		transform.position.y = p_transform->position.y + 50;
	//		transform.position.z = p_transform->position.z + u * 10.0f;
	//		temp_entity = rECS.createEntity(transform, unit, idle_state); //
	//		army.unitIDs.push_back(temp_entity->getID());
	//	}
	//	//Create the user entity
	//	rECS.createEntity(army);
	//	//Clear the army vector before we start creating the next players army.
	//	army.unitIDs.clear();
	//}
}
//
//int2 FindStartingTile(PLAYER Id)
//{
//	int rows = ARENA_ROWS;
//	int columns = ARENA_COLUMNS;
//	int2 index;
//	index.x = -1;
//	index.y = -1;
//	GridProp* p_gp = GridProp::GetInstance();
//	bool tile_found = false;
//	switch (Id)
//	{
//	case PLAYER1:
//		for (int y = 0; y < rows / 2; y++)
//		{
//			for (int x = 0; x < columns / 2; x++)
//			{
//				if (p_gp->mGrid[y][x].isPassable)
//				{
//					index.x = x;
//					index.y = y;
//					return index;
//				}
//			}
//		}
//		break;
//	case PLAYER2:
//		for (int y = 0; y < rows / 2; y++)
//		{
//			for (int x = columns - 1; x > columns / 2; x--)
//			{
//				if (p_gp->mGrid[y][x].isPassable)
//				{
//					index.x = x;
//					index.y = y;
//					return index;
//				}
//			}
//		}
//		break;
//	case PLAYER3:
//		for (int y = rows - 1; y > rows / 2; y--)
//		{
//			for (int x = 0; x < columns / 2; x++)
//			{
//				if (p_gp->mGrid[y][x].isPassable)
//				{
//					index.x = x;
//					index.y = y;
//					return index;
//				}
//			}
//		}
//		break;
//	case PLAYER4:
//		for (int y = rows - 1; y > rows / 2; y--)
//		{
//			for (int x = columns - 1; x > columns / 2; x--)
//			{
//				if (p_gp->mGrid[y][x].isPassable)
//				{
//					index.x = x;
//					index.y = y;
//					return index;
//				}
//			}
//		}
//		break;
//	default:
//		break;
//	}
//	return index;
//}