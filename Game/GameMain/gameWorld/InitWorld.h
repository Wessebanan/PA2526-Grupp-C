#pragma once

#include <DirectXMath.h>
#include "ecs.h"
#include "Mesh.h"

#include "WorldSettings.h"
#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"

using namespace ecs;
using namespace DirectX;

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

	WorldVertex() : position(0.f, 0.f, 0.f), color(0,0,0,0) {}
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