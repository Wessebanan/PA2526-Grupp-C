#pragma once

#include <vector>
#include <DirectXMath.h>
#include <iostream>

#include "ecs.h"
#include "..\\gameUtility\\UtilityComponents.h"
#include "..\\gameAI\\AIComponents.h"
#include "OceanSettings.h"

using namespace ecs;
using namespace DirectX;
using namespace std;

void InitOcean(EntityComponentSystem& rEcs)
{
	/*
		Fetch all existing tiles we want to create ocean around
	*/
	//TypeFilter tile_filter;
	//tile_filter.addRequirement(components::TransformComponent::typeID);
	//tile_filter.addRequirement(components::TileComponent::typeID);
	//EntityIterator tile_entity_iterator = rEcs.getEntititesByFilter(tile_filter);
	//
	//cout << "\n\n\n\n\n" << endl;
	//for (FilteredEntity fe : tile_entity_iterator.entities)
	//{
	//	TransformComponent* p_transform = fe.getComponent<TransformComponent>();
	//	cout << "Tile position - x:" << p_transform->position.x << "\t" <<
	//		"y:" << p_transform->position.y << "\t" <<
	//		"z:" << p_transform->position.z << endl;
	//}

	//getchar();

	/*
		This function will create an ocean around the game map.
		
		Version 1: Create a filled circle of tile meshes around origo.
	*/
	std::vector<XMVECTOR> ocean_tile_positions;

	unsigned int ocean_tile_count = 0;

	const float MID_TO_SIDE = cos(30.f * 3.1415f / 180.f);
	//XMFLOAT3 starting_pos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 starting_pos =
	{
		-(OCEAN_ROWS_MAX /2) * (1.5f),
		-1.0f,
		-(OCEAN_ROWS_MAX /2) * MID_TO_SIDE * 2 + (((OCEAN_COLUMNS_MAX / 2) % 2) ? MID_TO_SIDE : 0)
	};
	XMFLOAT3 current_pos = { 0.0f, 0.0f, 0.0f };

	const XMFLOAT3 OCEAN_CENTER = { (OCEAN_ALIGN_TO_MAP_ROWS / 2) * (1.5f), -1.f, (OCEAN_ALIGN_TO_MAP_ROWS / 2) * MID_TO_SIDE * 2 + (((OCEAN_ALIGN_TO_MAP_COLUMNS / 2) % 2) ? MID_TO_SIDE : 0) };
	OceanTileComponent ocean_tile;
	TransformComponent transform;
	ColorComponent color;
	color.red = 0;
	color.green = 0;

	current_pos = starting_pos;
	transform.position = starting_pos;

	XMVECTOR xm_center = XMLoadFloat3(&OCEAN_CENTER);

	//Calculate each tile position and create them
	for (int i = 0; i < OCEAN_ROWS_MAX; i++)
	{
		for (int j = 0; j < OCEAN_COLUMNS_MAX; j++)
		{
			// Set position of tile
			transform.position.x = starting_pos.x + j * 1.5f;
			transform.position.z = starting_pos.z + i * MID_TO_SIDE * 2 + ((j % 2) ? MID_TO_SIDE : 0);

			XMVECTOR xm_pos = XMLoadFloat3(&transform.position);

			XMVECTOR xm_length = XMVector3Length(xm_pos - xm_center);

			// Randomize a slightly different blue color for each ocean tile
			int variation = rand() % 155 + 1;
			color.blue = 100 + variation;

			if (XMVectorGetX(xm_length) > OCEAN_RADIUS)
			{
				transform.position.x = 100000.f;
			}

			//Create the new entity
			if (ocean_tile_count < OCEAN_TILE_COUNT)
			{
				Entity* p_entity = rEcs.createEntity(transform, color, ocean_tile);
				ocean_tile_count++;

				if (XMVectorGetX(xm_length) <= OCEAN_RADIUS)
				{
					ocean_tile_positions.push_back(xm_pos);
				}
			}
		}
	}

	int a = 0;
}