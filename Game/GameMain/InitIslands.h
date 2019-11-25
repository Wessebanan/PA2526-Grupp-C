#pragma once
#include "ecs.h"

#include "gameUtility/UtilityComponents.h"
#include "gameAI/AIComponents.h"

#define ISLAND_TILE_COUNT 16

struct IslandTiles
{
	TransformComponent center;
	TransformComponent transform[ISLAND_TILE_COUNT];
};

void InitIslands(ecs::EntityComponentSystem& rECS)
{
	IslandTiles island;
	ColorComponent color;
	TileComponent tile;

	// Spacing inbetween the tile in the island
	const float TILE_SPACING_X = (TILE_RADIUS * 1.5f);
	const float TILE_SPACING_Z = 1.8f;

	// The default color, this will be applied to tiles without any biome
	color.red = 190;
	color.green = 190;
	color.blue = 40;

	tile.tileType = TileTypes::UNDEFINED;
	tile.impassable = true;
	tile.goal = false;

	for (size_t i = 0; i < 10; i++)
	{
		/*
			1
		0		2
			3
		*/

		for (size_t i = 0; i < ISLAND_TILE_COUNT; i++)
			island.transform[i].position = island.center.position;

		XMFLOAT3 start_pos = {-19.5f, -0.3f, -20.0f};

		int rand_side = rand() % 6;
		switch (rand_side)
		{
		case 0:
		case 5:
			start_pos.z = (-19.0f) + (TILE_RADIUS * (int)(rand() % 30));
			start_pos.x = (-19.5f) - (TILE_RADIUS * (int)(rand() % 30));
			break;
		case 1:
			start_pos.z = (29.5f) + (TILE_RADIUS * (int)(rand() % 30));
			start_pos.x = (29.5f) - (TILE_RADIUS * (int)(rand() % 30));
			break;
		case 2:
			start_pos.z = (29.0f) - (TILE_RADIUS * (int)(rand() % 30));
			start_pos.x = (29.5f) + (TILE_RADIUS * (int)(rand() % 30));
			break;
		case 3:
		case 4:
			start_pos.z = (-9.5f) - (TILE_RADIUS * (int)(rand() % 30));
			start_pos.x = (-19.0f) + (TILE_RADIUS * (int)(rand() % 30));
			break;
		default:
			break;
		}

		XMFLOAT3 curr_pos = start_pos;
		int index = 0;

		int rand_island = rand() % 3;
		//switch (rand_side)
		switch (rand_island)
		{
			/*
			===================================
			===========ISLAND ONE==============
			===================================
			*/
		case 0:
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			// NEXT ROW
			curr_pos.y = start_pos.y + 0.2f;
			curr_pos.z = start_pos.z + 0.9f;
			curr_pos.x += TILE_SPACING_X;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			// NEXT ROW
			curr_pos.z = start_pos.z;
			curr_pos.x += TILE_SPACING_X;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			// NEXT ROW
			curr_pos.y = start_pos.y + 0.0f;
			curr_pos.z = start_pos.z + 0.9f;
			curr_pos.x += TILE_SPACING_X;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;
			break;
			/*
			===================================
			===========ISLAND TWO==============
			===================================
			*/
		case 1:
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			// NEXT ROW
			curr_pos.y = start_pos.y + 0.2f;
			curr_pos.z = start_pos.z + 0.9f;
			curr_pos.x += TILE_SPACING_X;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			// NEXT ROW
			curr_pos.z = start_pos.z;
			curr_pos.x += TILE_SPACING_X;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			// NEXT ROW
			curr_pos.y = start_pos.y + 0.0f;
			curr_pos.z = start_pos.z + 0.9f;
			curr_pos.x += TILE_SPACING_X;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			// NEXT ROW
			curr_pos.z = start_pos.z;
			curr_pos.x += TILE_SPACING_X;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;
			break;
			/*
			===================================
			===========ISLAND THREE============
			===================================
			*/
		case 2:
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			// NEXT ROW
			curr_pos.y = start_pos.y + 0.2f;
			curr_pos.z = start_pos.z + 0.9f;
			curr_pos.x += TILE_SPACING_X;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			// NEXT ROW
			curr_pos.z = start_pos.z;
			curr_pos.x += TILE_SPACING_X;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			// NEXT ROW
			curr_pos.y = start_pos.y + 0.0f;
			curr_pos.z = start_pos.z + 0.9f - (TILE_RADIUS*1.5f);
			curr_pos.x += TILE_SPACING_X;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			// NEXT ROW
			curr_pos.z = start_pos.z;
			curr_pos.x += TILE_SPACING_X;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;

			curr_pos.z += TILE_SPACING_Z;
			island.transform[index].position = curr_pos;
			index++;
			break;
		default:
			break;
		}

		
		SceneObjectComponent scene_comp;
		ColorComponent obj_color_cmp;
		TransformComponent obj_transform_comp;

		obj_color_cmp.red = rand() % 130 + 50;
		obj_color_cmp.green = rand() % 130 + 50;
		obj_color_cmp.blue = rand() % 130 + 50;
		scene_comp.mObjectType = OBJECT_TYPE::IMPASSABLE;

		float r = rand();
		scene_comp.mObject = GAME_OBJECT_TYPES::GAME_OBJECT_TYPE_CACTUS;
		obj_transform_comp.position = island.transform[(int)(rand() % ISLAND_TILE_COUNT)].position;
		obj_transform_comp.rotation.y = (r / (float)RAND_MAX) * 3.14f * 2.0f;
		rECS.createEntity(scene_comp, obj_transform_comp, obj_color_cmp);
		
		r = rand();
		scene_comp.mObject = GAME_OBJECT_TYPES::GAME_OBJECT_TYPE_FRUITTREE;
		obj_transform_comp.position = island.transform[(int)(rand() % ISLAND_TILE_COUNT)].position;
		obj_transform_comp.rotation.y = (r / (float)RAND_MAX) * 3.14f * 2.0f;
		rECS.createEntity(scene_comp, obj_transform_comp, obj_color_cmp);


		for (size_t i = 0; i < ISLAND_TILE_COUNT; i++)
		{
			color.red = 190 + (rand() % 55);
			color.green = 190 + (rand() % 55);
			color.blue = 5 + (rand() % 35);

			rECS.createEntity(island.transform[i], color, tile);
		}
	}
	
}