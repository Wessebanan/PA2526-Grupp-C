#pragma once

#include "ecs.h"

#include "../../AI/AI/includes/GridProp.h"
#include "..//gameAI/GridEcsFunctions.h"
#include "..//gameAI/AIComponents.h"
#include "..//gameUtility/UtilityComponents.h"
#include <DirectXMath.h>


void InitGrid(ecs::EntityComponentSystem& rECS)
{
	GridEcsFunctions::CreateGrid(rECS, 12, 12, 1.0f);

	//using namespace DirectX;

	//unsigned int rows = ARENA_ROWS;
	//unsigned int columns = ARENA_COLUMNS;
	//float  radius = TILE_RADIUS;

	//float pi = 3.1415f;
	//XMFLOAT3 starting_pos = { 0.0f, 0.0f, 0.0f };
	//XMFLOAT3 current_pos = { 0.0f, 0.0f, 0.0f };
	//float mid_to_side = cos(30 * pi / 180) * radius; //Calculate length between the center position and a side. 
	//TransformComponent transform;
	//TileComponent tile;
	//ecs::Entity* current_tile;
	//float height_map[ARENA_COLUMNS * ARENA_ROWS];
	//
	//float height_values[144] =
	//{ 0.f,0.f,0.f,0.f,1.f,1.f,1.f,0.f,0.f,0.f,0.f,0.f,
	//  0.f,0.f,0.f,0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,0.f,
	//  0.f,0.f,0.f,0.f,0.f,1.f,1.f,1.f,0.f,0.f,0.f,0.f,
	//  0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	//  -1.f,-1.f,-1.f,-1.f,-1.f,0.f,0.f,-1.f,-1.f,-1.f,-1.f,-1.f,
	//  0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	//  0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	//  0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,0.f,2.f,0.f,0.f,
	//  0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,0.f,2.f,0.f,0.f,
	//  0.f,1.f,2.f,3.f,2.f,1.f,0.f,0.f,-2.f,0.f,0.f,0.f,
	//  0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	//  0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };

	//for (int i = 0; i < 144; i++)
	//{
	//	height_map[i] = height_values[i];
	//}


	//GridProp* p_gp = GridProp::GetInstance();
	////Calculate the position and create every tile.
	//for (int i = 0; i < rows; i++)
	//{
	//	//Reset x-pos for every new row and set the starting z-pos for the new row.
	//	current_pos.x = starting_pos.x;
	//	current_pos.z = starting_pos.z + i * mid_to_side * 2;
	//	for (int j = 0; j < columns; j++)
	//	{

	//		//Save the calculated values into the PositionComponent.
	//		transform.position.x = current_pos.x;
	//		transform.position.y = height_map[(i * 12) + j];
	//		transform.position.z = current_pos.z;
	//		if (transform.position.y == -1.f)
	//		{
	//			tile.tileType = WATER;
	//			tile.impassable = true;
	//			tile.goal = false;
	//			p_gp->mGrid[i][j].isPassable = false;
	//		}
	//		else if (transform.position.y == 3)
	//		{
	//			tile.tileType = STONE;
	//			tile.impassable = false;
	//			tile.goal = false;
	//			p_gp->mGrid[i][j].isPassable = true;
	//		}
	//		else if (transform.position.y == -2)
	//		{
	//			tile.tileType = WATER;
	//			tile.impassable = false;
	//			p_gp->mGrid[i][j].isPassable = true;
	//			tile.goal = true;
	//		}
	//		else
	//		{
	//			tile.tileType = GRASS;
	//			tile.impassable = false;
	//			tile.goal = false;
	//			p_gp->mGrid[i][j].isPassable = true;
	//		}

	//		//Create the new entity
	//		current_tile = rECS.createEntity(transform, tile);
	//		p_gp->mGrid[i][j].Id = current_tile->getID();
	//		p_gp->mGrid[i][j].height = transform.position.y;
	//		//Update the x-position of the next tile in this row.
	//		current_pos.x += 1.5f * radius;
	//		//Update the z-position of the next tile depending on if it is in a 
	//		//odd or even column.
	//		if (j % 2 == 0)
	//		{
	//			current_pos.z += mid_to_side;
	//		}
	//		else
	//		{
	//			current_pos.z -= mid_to_side;
	//		}
	//	}
	//}

}