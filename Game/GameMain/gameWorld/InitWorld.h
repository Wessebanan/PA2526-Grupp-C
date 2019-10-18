#pragma once

#include <DirectXMath.h>
#include "ecs.h"
#include "Mesh.h"

#include "WorldSettings.h"
#include "WorldComponents.h"

#include "../../AI/AI/includes/AIGlobals.h"
#include "../gameAI/AIComponents.h"

#include "../gameUtility/UtilityComponents.h"

#include "../../../Graphics/includes/MeshManager.h"

using namespace ecs;
using namespace DirectX;

/*
	Generating functions
*/
void InitWorldTiles(EntityComponentSystem& rEcs, graphics::MeshManager& rMeshMgr);
void CreateMapTilesInEcs(EntityComponentSystem& rEcs);
void CreateOceanTilesInEcs(EntityComponentSystem& rEcs);

/*
	Help functions
*/
XMFLOAT3 GetCenterMapTilePosition(EntityComponentSystem& rEcs);






/*
	Generating functions
*/
void InitWorldTiles(EntityComponentSystem& rEcs, graphics::MeshManager& rMeshMgr)
{

}

void CreateMapTilesInEcs(EntityComponentSystem& rEcs)
{

}

void CreateOceanTilesInEcs(EntityComponentSystem& rEcs)
{

}







/*
	Help functions
*/
XMFLOAT3 GetCenterMapTilePosition(EntityComponentSystem& rEcs)
{
	TypeFilter tile_filter;
	tile_filter.addRequirement(components::TransformComponent::typeID);
	tile_filter.addRequirement(components::TileComponent::typeID);
	EntityIterator tile_entity_iterator = rEcs.getEntititesByFilter(tile_filter);

	/*
		-- Explanation
		First we calculate the average position of all map tiles. But in order to
		start generate ocean tiles based on this position, we have to find the exact
		TILE position of the most-center map tile.
	*/

	// Find the center position of generated map
	XMVECTOR xm_exact_map_center = XMVectorZero();

	for (FilteredEntity tile : tile_entity_iterator.entities)
	{
		TransformComponent* p_transform = tile.getComponent<TransformComponent>();
		xm_exact_map_center = XMVectorAdd(xm_exact_map_center, XMLoadFloat3(&p_transform->position));
	}
	xm_exact_map_center = XMVectorScale(xm_exact_map_center, (float)1.f / ((float)tile_entity_iterator.entities.size()));



	// Find the most-center map tile
	XMFLOAT3 center_tile_position = { 10000000000.f, 10000000000.f ,10000000000.f };			// Large xyz in order for first iteration to write over it
	XMVECTOR xm_center = XMLoadFloat3(&center_tile_position);									// XMVECTOR used for DX math functions
	float closest_distance = XMVectorGetX(XMVector3Length(xm_center - xm_exact_map_center));	// Distance used to determine center tile

	for (FilteredEntity tile : tile_entity_iterator.entities)
	{
		TransformComponent* p_transform = tile.getComponent<TransformComponent>();
		XMVECTOR xm_current_position = XMLoadFloat3(&p_transform->position);

		float distance = XMVectorGetX(XMVector3Length(xm_current_position - xm_exact_map_center));
		if (distance < closest_distance)
		{
			xm_center = xm_current_position;
			closest_distance = distance;
		}
	}
	XMStoreFloat3(&center_tile_position, xm_center);


	/*
		Generate ocean in a disk shape arounce center tile.
		We try to generate a large GRID of ocean tiles, like a rectangle, but for each tile
		we check its distance to map center. If the distance is larger than OCEAN_RADIUS, 
		we don't generate that ocean tile. This results in a disk-shape ocean.
	*/

	// Create necessary components to create ocean tile entities
	ecs::components::ColorComponent color;
	ecs::components::OceanTileComponent tile;
	ecs::components::TransformComponent transform;

	// Pre set red and green channel in color, as the color between tiles only differ in blue channel
	color.red = 0;
	color.green = 0;

	// Pre set y-position of tile
	transform.position.y = OCEAN_START_HEIGHT;

	// Calculate rows and columns of the grid, based on OCEAN_RADIUS
	const int OCEAN_GRID_ROWS = (int)ceilf((float)OCEAN_RADIUS / HEXA_TILE_MID_TO_SIDE * 0.75f);
	const int OCEAN_GRID_COLUMNS = OCEAN_GRID_ROWS;

	/*
		Based on grid rows and columns, calculate start position that is one corner of the grid.
		Later, when iterating all possible positions, offsets are calculated and added on this
		starting position in order to find each position.
	*/
	const XMFLOAT3 GRID_START_POS =
	{
		center_tile_position.x - (OCEAN_GRID_ROWS / 2) * (1.5f),
		OCEAN_START_HEIGHT,
		center_tile_position.z - (OCEAN_GRID_ROWS / 2) * HEXA_TILE_MID_TO_SIDE * 2 + (((OCEAN_GRID_COLUMNS / 2) % 2) ? HEXA_TILE_MID_TO_SIDE : 0)
	};

	// Iterate possible grid positions
	for (int i = 0; i < OCEAN_GRID_ROWS; i++)
	{
		for (int j = 0; j < OCEAN_GRID_COLUMNS; j++)
		{
			// Calculate current position
			transform.position.x = GRID_START_POS.x + j * 1.5f;
			transform.position.y = OCEAN_START_HEIGHT;
			transform.position.z = GRID_START_POS.z + i * HEXA_TILE_MID_TO_SIDE * 2 + ((j % 2) ? HEXA_TILE_MID_TO_SIDE : 0);

			// Store current position in a XMVECTOR for math operations
			XMVECTOR xm_position = XMLoadFloat3(&transform.position);

			// Calculate distance to center
			float distance_to_center = XMVectorGetX(XMVector3Length(xm_position - xm_center));

			// Determine if current position is valid. If not, continue to next position
			if (distance_to_center > OCEAN_RADIUS)
			{
				continue; // Skips rest of inner loop iteration
			}

			// Randomize a blue color
			int variation = rand() % 155 + 1;
			color.blue = 100 + variation;

			rEcs.createEntity(transform, color, tile);
		}
	}
}










// ###############################################################################################################################################
// ###############################################################################################################################################
// ###############################################################################################################################################
// ###############################################################################################################################################
// ###############################################################################################################################################
// ###############################################################################################################################################
// ###############################################################################################################################################
// ###############################################################################################################################################


/*
	This struct defines each vertex in the world 'mesh' vertex buffer. Each vertex
	includes a position (FLOAT3) and a color (4 * UINT_8).
*/
struct WorldVertex
{
	XMFLOAT3 position;

	struct Color
	{
		UINT red : 8;
		UINT green : 8;
		UINT blue : 8;
		UINT alpha : 8;
		Color(UINT _red, UINT _green, UINT _blue, UINT _alpha) : red(_red), green(_green), blue(_blue), alpha(_alpha) {}
	} color;

	WorldVertex() : position(0.f, 0.f, 0.f), color(0, 0, 0, 0) {}
	WorldVertex(XMFLOAT3 pos, UINT red, UINT green, UINT blue, UINT alpha) :
		position(pos), color(red, green, blue, alpha) {}
};

/*
	This struct defines each vertex in the world 'mesh' vertex buffer.
*/
struct TileVertexBuffer
{
	WorldVertex* pFirst;
	unsigned int vertexCount;
};

/*
	This struct defines a look-up table, used by the compute shader that updates the ocean grid. The compute shader
	need to know
		1. The number of ocean tiles
		2. Where in the vertex buffer the ocean tiles starts (map tiles are placed at the beginning)
		3. The X and Z position values for each ocean tile, where the main task of the shader is to calculate Y
		   and place each ocean tile vertex accordingly.
*/
struct OceanShaderLookUpTable
{
	XMFLOAT2* pPositionsXZ;		// Positions of ocean tiles in the X and Z plane, Y is skipped as it will be calculated by a compute shader
	UINT oceanTileCount;		// Number of ocean tiles
	UINT oceanTileOffset;		// Offset in vertex buffer where ocean tiles start
};

/*
	This function generates all necessary information to render the world tiles (map + ocean) as one mesh,
	including the information necessary for updating the ocean tiles.
*/
void CreateWorldTileVertexBuffer(EntityComponentSystem& rEcs, ModelLoader::Mesh* p_tile_mesh, TileVertexBuffer** pp_vertex_buffer_output)
{
	/*
		TODO: Change vertex buffer to use index buffer
	*/

	// Fetch mesh data
	std::vector<XMFLOAT3>* p_mesh_vertices = p_tile_mesh->GetVertexPositionVector();
	std::vector<int>* p_mesh_indices = p_tile_mesh->GetIndexVector();

	/*
		Create vertex buffer for all tiles (map + ocean), and a look up
		table used later in a compute shader that will update ocean tile
		positions during runtime.
	*/
	TileVertexBuffer* p_vertex_buffer = new TileVertexBuffer;
	OceanShaderLookUpTable* p_look_up_table = new OceanShaderLookUpTable;

	// Fetch all existing tile entities
	TypeFilter type_filter;
	type_filter.addRequirement(components::TileComponent::typeID);
	type_filter.addRequirement(components::ColorComponent::typeID);
	type_filter.addRequirement(components::TransformComponent::typeID);
	EntityIterator tile_iterator = rEcs.getEntititesByFilter(type_filter);

	// Calculate total tile count (world tiles + ocean tiles)
	const UINT total_tile_count = (unsigned int)tile_iterator.entities.size() + OCEAN_TILE_COUNT;
	UINT inited_tile_count = 0;

	// Create vertex array and lookup table array
	p_vertex_buffer->pFirst = new WorldVertex[total_tile_count * p_mesh_indices->size()];
	p_look_up_table->pPositionsXZ = new XMFLOAT2[OCEAN_TILE_COUNT];



	/*
		Init all world tiles
	*/

	// Declare temporary storage variables for calculations
	XMFLOAT3 position;
	XMVECTOR xm_position;
	XMMATRIX xm_world;
	components::ColorComponent* p_color;
	components::TransformComponent* p_transform;

	// Iterate all map tiles and place their vertices in vertex buffer
	for (FilteredEntity& tile : tile_iterator.entities)
	{
		// Fetch world position and color of current tile
		p_color = tile.getComponent<components::ColorComponent>();
		p_transform = tile.getComponent<components::TransformComponent>();

		// Calculate world matrix for tile
		xm_world = XMMatrixTranslation(p_transform->position.x, p_transform->position.y, p_transform->position.z);

		/*
			Iterate all vertices in the mesh,
			translate them to world position,
			set vertex color,
			push vertex at the back of vertex buffer
		*/
		for (int index : *p_mesh_indices)
		{
			std::vector<XMFLOAT3> vert = *p_mesh_vertices;
			XMFLOAT3 pos = vert[index];
			xm_position = XMLoadFloat3(&(*p_mesh_vertices)[index]);
			xm_position = XMVector3Transform(xm_position, xm_world);
			XMStoreFloat3(&position, xm_position);

			p_vertex_buffer->pFirst[inited_tile_count++] = WorldVertex(position, p_color->red, p_color->green, p_color->blue, 255);
		}
	}

	// Make look up table know where ocean tiles start
	p_look_up_table->oceanTileOffset = inited_tile_count;



	/*
		Init all ocean tiles
	*/
	type_filter.removeRequirement(components::TileComponent::typeID);
	type_filter.addRequirement(components::OceanTileComponent::typeID);
	tile_iterator = rEcs.getEntititesByFilter(type_filter);
	UINT ocean_tile_counter = 0;

	// Iterate all ocean tiles and place their vertices in vertex buffer
	for (FilteredEntity& tile : tile_iterator.entities)
	{
		// Fetch world position and color of current tile
		p_color = tile.getComponent<components::ColorComponent>();
		p_transform = tile.getComponent<components::TransformComponent>();

		// Calculate world matrix for tile
		xm_world = XMMatrixTranslation(p_transform->position.x, p_transform->position.y, p_transform->position.z);

		// Push XZ position at back of look up table, used later in compute shader for water effects
		p_look_up_table->pPositionsXZ[ocean_tile_counter++] = XMFLOAT2(p_transform->position.x, p_transform->position.z);

		/*
			Iterate all vertices in the mesh,
			translate them to world position,
			set vertex color,
			push vertex at the back of vertex buffer
		*/
		for (int index : *p_mesh_indices)
		{
			std::vector<XMFLOAT3> vert = *p_mesh_vertices;
			XMFLOAT3 pos = vert[index];
			xm_position = XMLoadFloat3(&(*p_mesh_vertices)[index]);
			xm_position = XMVector3Transform(xm_position, xm_world);
			XMStoreFloat3(&position, xm_position);

			p_vertex_buffer->pFirst[inited_tile_count++] = WorldVertex(position, p_color->red, p_color->green, p_color->blue, 255);
		}
	}

	p_vertex_buffer->vertexCount = inited_tile_count;
	p_look_up_table->oceanTileCount = ocean_tile_counter;

	// Delete this. These are only used to free memory until the new graphics engine exist
	delete p_vertex_buffer->pFirst;
	delete p_vertex_buffer;
	delete p_look_up_table->pPositionsXZ;
	delete p_look_up_table;

	*pp_vertex_buffer_output = p_vertex_buffer;
}