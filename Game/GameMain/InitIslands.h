#pragma once
#include "ecs.h"

#include "gameUtility/UtilityComponents.h"
#include "gameAI/AIComponents.h"

#define ISLAND_TILE_COUNT 12

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
	const float TILE_SPACING_X = 1.2f;
	const float TILE_SPACING_Z = 1.2f;

	// The default color, this will be applied to tiles without any biome
	color.red = 200;
	color.green = 200;
	color.blue = 50;

	for (size_t i = 0; i < 1; i++)
	{
		//Save the calculated values into the PositionComponent.
		island.center.position.x = -20.0f + ((rand() % 10) - 5);
		island.center.position.y = 0.2f;
		island.center.position.z = 20.0f + ((rand() % 10) - 5);

		tile.tileType = TileTypes::GAME_FIELD;
		tile.impassable = true;
		tile.goal = false;


		/*
		0	1	2
		3	4	5
		6	7	8
		9	10	11
		*/

		for (size_t i = 0; i < ISLAND_TILE_COUNT; i++)
			island.transform[i].position = island.center.position;


		/*const int radius = 15;
		for (size_t i = 0; i < ISLAND_TILE_COUNT; i++)
		{
			island.transform[i].position.y = island.center.position.y +
				(float)((float)((rand() % 10) - 5) / 6.0f);


			island.transform[i].position.x = island.center.position.x + 
				(float)((float)((rand() % (radius * 2)) - radius) / 4.5f);
			
			island.transform[i].position.z = island.center.position.z + 
				(float)((float)((rand() % (radius * 2)) - radius) / 4.5f);
		}*/

		int index = 0;

		island.transform[index].position.x = island.center.position.x - TILE_SPACING_X;
		island.transform[index].position.z = island.center.position.z - TILE_SPACING_Z;

		index++;
		island.transform[index].position.z = island.center.position.z - TILE_SPACING_Z;

		index++;
		island.transform[index].position.x = island.center.position.x + TILE_SPACING_X;
		island.transform[index].position.z = island.center.position.z - TILE_SPACING_Z;


		index++;
		island.transform[index].position.x = island.center.position.x - TILE_SPACING_X;

		index++;
		island.transform[index].position.y = island.center.position.y + 0.2f;

		index++;
		island.transform[index].position.x = island.center.position.x + TILE_SPACING_X;


		index++;
		island.transform[index].position.x = island.center.position.x - TILE_SPACING_X;
		island.transform[index].position.z = island.center.position.z + TILE_SPACING_Z;

		index++;
		island.transform[index].position.z = island.center.position.z + TILE_SPACING_Z;

		index++;
		island.transform[index].position.x = island.center.position.x + TILE_SPACING_X;
		island.transform[index].position.z = island.center.position.z + TILE_SPACING_Z;

		// 9-11
		index++;
		island.transform[index].position.x = island.center.position.x - TILE_SPACING_X;
		island.transform[index].position.z = island.center.position.z + TILE_SPACING_Z + TILE_SPACING_Z;

		index++;
		island.transform[index].position.z = island.center.position.z + TILE_SPACING_Z + TILE_SPACING_Z;

		index++;
		island.transform[index].position.x = island.center.position.x + TILE_SPACING_X;
		island.transform[index].position.z = island.center.position.z + TILE_SPACING_Z + TILE_SPACING_Z;


		for (size_t i = 0; i < ISLAND_TILE_COUNT; i++)
			rECS.createEntity(island.transform[i], color, tile);
	}
	
}