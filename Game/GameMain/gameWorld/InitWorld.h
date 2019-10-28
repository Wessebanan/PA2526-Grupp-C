#pragma once

#include "ecs.h"

#include "WorldSettings.h"
#include "OceanComponents.h"

#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../MeshContainer/MeshContainer.h"

using namespace DirectX;
using namespace ecs;
using namespace ecs::components;

TransformComponent* GetCenterMapTileTransform(EntityComponentSystem& rEcs)
{
	// Fetch all existing tile entities
	TypeFilter type_filter;
	type_filter.addRequirement(components::TileComponent::typeID);
	type_filter.addRequirement(components::ColorComponent::typeID);
	type_filter.addRequirement(components::TransformComponent::typeID);
	EntityIterator tile_iterator = rEcs.getEntititesByFilter(type_filter);

	/*
		Store exact map center in a XMVECTOR, so we can search all map tiles later and
		recieve the tile closest to the exact map center.
	*/
	XMVECTOR exact_map_center = XMVectorZero();
	for (FilteredEntity fe : tile_iterator.entities)
	{
		TransformComponent* p_transform = fe.getComponent<TransformComponent>();
		exact_map_center = XMVectorAdd(exact_map_center, XMLoadFloat3(&p_transform->position));
	}
	exact_map_center = XMVectorScale(exact_map_center, (float)1.f / ((float)tile_iterator.entities.size()));

	/*
		Iterate all tiles to find the most-center one
	*/

	TransformComponent* p_most_center_transform = nullptr;
	XMFLOAT3 most_center_position = { 10000.f, 10000.f ,10000.f };
	XMVECTOR xm_most_center_position = XMLoadFloat3(&most_center_position); // Use XMVECTOR for calculations
	float closest_distance_to_center = XMVectorGetX(XMVector3Length(xm_most_center_position - exact_map_center));

	for (FilteredEntity fe : tile_iterator.entities)
	{
		TransformComponent* p_transform = fe.getComponent<TransformComponent>();
		XMVECTOR xm_current_position = XMLoadFloat3(&p_transform->position);
		float distance = XMVectorGetX(XMVector3Length(xm_current_position - exact_map_center));
		if (distance < closest_distance_to_center)
		{
			xm_most_center_position = xm_current_position;
			closest_distance_to_center = distance;
			p_most_center_transform = p_transform;
		}
	}
	return p_most_center_transform;
}

void InitOceanEntities(EntityComponentSystem& rEcs)
{
	/*
		Generates tiles for the ocean, using hexa-grid positions.

		First, we find the center of the generated map, then we use that
		position as a center to generate the ocean around.

		In order to calculate ocean tile positions, we generate positions
		for a rectangle grid and only use those within OCEAN_RADIUS (located
		in WorldSettings.h) from center.
	*/

	TransformComponent* p_center_map_transform = GetCenterMapTileTransform(rEcs);
	XMFLOAT3 center = p_center_map_transform->position;
	XMVECTOR xm_center = XMLoadFloat3(&center);

	const float MID_TO_SIDE = cos(30.f * 3.1415f / 180.f);
	const int OCEAN_ROWS = (int)ceilf((float)OCEAN_RADIUS / (MID_TO_SIDE * 0.75f));
	const int OCEAN_COLUMNS = OCEAN_ROWS;

	/*
		Set a position in one of the ROW/COLUMN grid corners, that we will use to calculate
		each grid position with.
	*/
	XMFLOAT3 starting_pos =
	{
		center.x - (OCEAN_ROWS / 2) * (1.5f),
		OCEAN_START_HEIGHT,
		center.z - (OCEAN_ROWS / 2) * MID_TO_SIDE * 2 + (((OCEAN_COLUMNS / 2) % 2) ? MID_TO_SIDE : 0)
	};

	// Create component descs for ocean tile entities
	OceanTileComponent ocean_tile_desc;
	TransformComponent transform_desc;
	ColorComponent color_desc;

	// Pre set red and green channel for ocean tiles
	color_desc.red = 0;
	color_desc.blue = 0;

	//Calculate each tile position and create them
	unsigned int ocean_tile_count = 0;
	for (int i = 0; i < OCEAN_ROWS; i++)
	{
		for (int j = 0; j < OCEAN_COLUMNS; j++)
		{
			// Set position of tile
			transform_desc.position.x = starting_pos.x + j * 1.5f;
			transform_desc.position.y = OCEAN_START_HEIGHT;
			transform_desc.position.z = starting_pos.z + i * MID_TO_SIDE * 2 + ((j % 2) ? MID_TO_SIDE : 0);

			XMVECTOR xm_pos = XMLoadFloat3(&transform_desc.position);

			float distance_to_center = XMVectorGetX(XMVector3Length(xm_pos - xm_center));

			// Randomize a slightly different blue color for each ocean tile
			int variation = rand() % 155 + 1;
			color_desc.blue = 100 + variation;

			//Create the new entity
			if (ocean_tile_count < OCEAN_TILE_COUNT_MAX && distance_to_center <= OCEAN_RADIUS)
			{
				rEcs.createEntity(ocean_tile_desc, transform_desc, color_desc);
				ocean_tile_count++;
			}
		}
	}
}