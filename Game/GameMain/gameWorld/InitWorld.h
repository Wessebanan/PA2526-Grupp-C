#pragma once

#include "ecs.h"

#include "WorldSettings.h"
#include "WorldComponents.h"
#include "../gameWorld/UpdateOceanSystem.h"

#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameUtility/UtilityGraphics.h"
#include "../MeshContainer/MeshContainer.h"

#include "../Renderers/Renderers.h"

#define Clamp(value, min_value, max_value) max(min_value, min(value, max_value))

using namespace DirectX;
using namespace ecs;
using namespace ecs::components;

static TransformComponent* GetCenterMapTileTransform(EntityComponentSystem& rEcs)
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

static void InitOceanEntities(EntityComponentSystem& rEcs)
{
	/*
		Generates tiles for the ocean, using hexa-grid positions.

		First, we find the center of the generated map, then we use that
		position as a center to generate the ocean around.

		In order to calculate ocean tile positions, we generate positions
		for a rectangle grid and only use those within OCEAN_RADIUS (located
		in WorldSettings.h) from center.
	*/
	TypeFilter map_filter;
	map_filter.addRequirement(TileComponent::typeID);
	map_filter.addRequirement(TransformComponent::typeID);
	EntityIterator map_tiles = rEcs.getEntititesByFilter(map_filter);

	std::vector<XMFLOAT3> map_positions;

	for (FilteredEntity& tile : map_tiles.entities)
	{
		if (tile.getComponent<TileComponent>()->tileType != WATER)
		{
			XMFLOAT3 pos = tile.getComponent<TransformComponent>()->position;
			pos.y = OCEAN_START_HEIGHT;
			map_positions.push_back(pos);
		}
	}

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

			bool underMap = false;
			for (XMFLOAT3 map_pos : map_positions)
			{
				if (XMVectorGetX(XMVector3Length(xm_pos - XMLoadFloat3(&map_pos))) <= 1.f)
				{
					underMap = true;
					break;
				}
			}

			if (underMap)
			{
				continue;
			}

			float distance_to_center = XMVectorGetX(XMVector3Length(xm_pos - xm_center));

			// Randomize a slightly different blue color for each ocean tile
			//int variation = rand() % 155 + 1;


			int random = rand() % 25;
			//int color_offset = -25 + random;

			color_desc.red = 31 - random;
			color_desc.green = 121 - random * 2;
			color_desc.blue = 255 - random * 3;

			//Create the new entity
			if (ocean_tile_count < OCEAN_TILE_COUNT_MAX && distance_to_center <= OCEAN_RADIUS)
			{
				rEcs.createEntity(ocean_tile_desc, transform_desc, color_desc);
				ocean_tile_count++;
			}
		}
	}
}

static void GenerateTileMesh(EntityComponentSystem& rEcs, void** pVertexBuffer, UINT& rVertexCount, TypeFilter& typeFilter)
{
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		UINT color;
	};

	/*
		Fetch the tile mesh, in order to make copies of it into the vertex buffer.
	*/

	std::vector<XMFLOAT3>& r_mesh_vertices = *MeshContainer::GetMeshCPU(GAME_OBJECT_TYPE_TILE)->GetVertexPositionVector();
	std::vector<XMFLOAT3>& r_mesh_normals = *MeshContainer::GetMeshCPU(GAME_OBJECT_TYPE_TILE)->GetNormalVector();
	std::vector<int>& r_mesh_indices = *MeshContainer::GetMeshCPU(GAME_OBJECT_TYPE_TILE)->GetIndexVector();

	/*
		Fetch iterators for both map tiles and ocean tiles, so we can iterate them and
		copy their positions in the world.
	*/

	EntityIterator tiles = rEcs.getEntititesByFilter(typeFilter);

	/*
		Store count variables, used to calculate size of buffer.
	*/

	UINT tile_count = (UINT)tiles.entities.size();

	// Create a vertex buffer to write all tile vertex data to
	Vertex* vertex_buffer = new Vertex[r_mesh_indices.size() * tile_count];
	UINT index_counter = 0;

	/*
		For now, instance_counter is mostly used for debug. With this, we know
		exacly how many tiles are generated in the vertex buffer.
		Instance_counter should become the same	number as total_tile_count.
	*/
	UINT instance_counter = 0;

	// Pre define variables used for calculations in entity loops
	XMVECTOR xm_pos;
	XMMATRIX xm_world;
	ColorComponent* p_color;
	TransformComponent* p_transform;

	/*
		Iterate all tiles, copy their location and place a whole mesh with world position
		in the vertex buffer.
	*/

	for (FilteredEntity& r_tile : tiles.entities)
	{
		/*
			Create color and world matrix, used for all vertices within this tile.
		*/

		p_color			= r_tile.getComponent<ColorComponent>();
		p_transform		= r_tile.getComponent<TransformComponent>();
		xm_world		= XMMatrixTranslation(p_transform->position.x, 0.f, p_transform->position.z);

		UINT islet_color;
		bool is_islet = r_tile.entity->hasComponentOfType<IsletComponent>();

		if (is_islet)
		{
			IsletComponent* p_islet_component = rEcs.getComponentFromEntity<IsletComponent>(r_tile.entity->getID());
			
			switch (p_islet_component->playerId)
			{
			case 0:
			{
				int offset = rand() % 100 - 50;
				islet_color = PACK(
					Clamp(PURPLE.r + offset, 0, 255),
					Clamp(PURPLE.g + offset, 0, 255),
					Clamp(PURPLE.b + offset, 0, 255),
					255);
				break;
			}

			case 1:
			{
				int offset = rand() % 100 - 50;
				islet_color = PACK(
					Clamp(RED.r + offset, 0, 255),
					Clamp(RED.g + offset, 0, 255),
					Clamp(RED.b + offset, 0, 255),
					255);
				break;
			}

			case 2:
			{
				int offset = rand() % 100 - 50;
				islet_color = PACK(
					Clamp(GREEN.r + offset, 0, 255),
					Clamp(GREEN.g + offset, 0, 255),
					Clamp(GREEN.b + offset, 0, 255),
					255);
				break;
			}

			case 3:
			{
				int offset = rand() % 100 - 50;
				islet_color = PACK(
					Clamp(BLUE.r + offset, 0, 255),
					Clamp(BLUE.g + offset, 0, 255),
					Clamp(BLUE.b + offset, 0, 255),
					255);
				break;
			}
			}
		}

		for (int i : r_mesh_indices)
		{
			xm_pos = XMLoadFloat3(&r_mesh_vertices[i]);
			xm_pos = XMVector3Transform(xm_pos, xm_world);

			XMStoreFloat3(&vertex_buffer[index_counter].position, xm_pos);
			vertex_buffer[index_counter].normal = r_mesh_normals[i];

			if (is_islet)
			{
				vertex_buffer[index_counter].color = islet_color;
			}
			else
			{
				vertex_buffer[index_counter].color = PACK(p_color->red, p_color->green, p_color->blue, 255);
			}

			index_counter++;
		}
	}

	*pVertexBuffer = (void*)vertex_buffer;
	rVertexCount = index_counter;
}

void InitOceanUpdateSystem(EntityComponentSystem& rEcs)
{
	 rEcs.createSystem<systems::UpdateOceanSystem>(7)->Initialize(0.001f);
}