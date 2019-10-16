#pragma once

#include <vector>
#include <DirectXMath.h>
#include <iostream>

#include "ecs.h"
#include "..\\gameUtility\\UtilityComponents.h"
#include "..\\gameAI\\AIComponents.h"

#include "Mesh.h"

#include "..\\gameWorld\\WorldSettings.h"

using namespace ecs;
using namespace DirectX;
using namespace std;

void InitOcean(EntityComponentSystem& rEcs)
{
	/*
		Fetch all existing tiles we want to create ocean around
	*/
	TypeFilter tile_filter;
	tile_filter.addRequirement(components::TransformComponent::typeID);
	tile_filter.addRequirement(components::TileComponent::typeID);
	EntityIterator tile_entity_iterator = rEcs.getEntititesByFilter(tile_filter);
	
	XMVECTOR xm_map_center = XMVectorZero();
	for (FilteredEntity fe : tile_entity_iterator.entities)
	{
		TransformComponent* p_transform = fe.getComponent<TransformComponent>();
		xm_map_center = XMVectorAdd(xm_map_center, XMLoadFloat3(&p_transform->position));
	}
	xm_map_center = XMVectorScale(xm_map_center, (float)1.f / ((float)tile_entity_iterator.entities.size()));

	// Get exact position of the center tile, in order to calculate a start position offset for the ocean
	XMFLOAT3 ocean_center = { 10000.f, 10000.f ,10000.f };
	XMVECTOR xm_ocean_center = XMLoadFloat3(&ocean_center);
	float closest_distance_to_center = XMVectorGetX(XMVector3Length(xm_ocean_center - xm_map_center));
	for (FilteredEntity fe : tile_entity_iterator.entities)
	{
		TransformComponent* p_transform = fe.getComponent<TransformComponent>();
		XMVECTOR xm_current_position = XMLoadFloat3(&p_transform->position);
		float distance = XMVectorGetX(XMVector3Length(xm_current_position - xm_map_center));
		if (distance < closest_distance_to_center)
		{
			xm_ocean_center = xm_current_position;
			closest_distance_to_center = distance;
		}
	}
	XMStoreFloat3(&ocean_center, xm_ocean_center);

	//getchar();

	/*
		This function will create an ocean around the game map.
		
		Version 1: Create a filled circle of tile meshes around origo.
	*/
	std::vector<XMVECTOR> ocean_tile_positions;

	unsigned int ocean_tile_count = 0;

	const float MID_TO_SIDE = cos(30.f * 3.1415f / 180.f);
	const int OCEAN_ROWS = (int)ceilf((float)OCEAN_RADIUS / (MID_TO_SIDE * 0.75f));
	const int OCEAN_COLUMNS = OCEAN_ROWS;

	//XMStoreFloat3(&ocean_center, xm_map_center);
	XMFLOAT3 starting_pos =
	{
		ocean_center.x - (OCEAN_ROWS /2) * (1.5f),
		OCEAN_START_HEIGHT,
		ocean_center.z - (OCEAN_ROWS /2) * MID_TO_SIDE * 2 + (((OCEAN_COLUMNS / 2) % 2) ? MID_TO_SIDE : 0)
	};

	OceanTileComponent ocean_tile;
	TransformComponent transform;
	ColorComponent color;
	color.red = 0;
	color.green = 0;

	transform.position = starting_pos;

	//Calculate each tile position and create them
	for (int i = 0; i < OCEAN_ROWS; i++)
	{
		for (int j = 0; j < OCEAN_COLUMNS; j++)
		{
			// Set position of tile
			transform.position.x = starting_pos.x + j * 1.5f;
			transform.position.y = OCEAN_START_HEIGHT;
			transform.position.z = starting_pos.z + i * MID_TO_SIDE * 2 + ((j % 2) ? MID_TO_SIDE : 0);

			XMVECTOR xm_pos = XMLoadFloat3(&transform.position);

			float distance_to_center = XMVectorGetX(XMVector3Length(xm_pos - xm_map_center));

			// Randomize a slightly different blue color for each ocean tile
			int variation = rand() % 155 + 1;
			color.blue = 100 + variation;

			//transform.position.y += (variation/255.f) * 0.8f - 0.4f;
			

			//Create the new entity
			if (ocean_tile_count < OCEAN_TILE_COUNT && distance_to_center <= OCEAN_RADIUS)
			{
				ocean_tile_positions.push_back(xm_pos);

				Entity* p_entity = rEcs.createEntity(transform, color, ocean_tile);

				ocean_tile_count++;
			}
		}
	}

	while (ocean_tile_count < OCEAN_TILE_COUNT)
	{
		transform.position.x = 100000.f;
		Entity* p_entity = rEcs.createEntity(transform, color, ocean_tile);
		ocean_tile_count++;
	}

	int a = 0;
}