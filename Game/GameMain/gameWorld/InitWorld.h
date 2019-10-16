#pragma once

#include <DirectXMath.h>
#include "ecs.h"
#include "Mesh.h"

#include "WorldSettings.h"
#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"

using namespace ecs;
using namespace DirectX;

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

struct TileVertexBuffer
{
	WorldVertex* first;
	unsigned int vertexCount;
};



void InitWorldTiles(EntityComponentSystem& rEcs, ModelLoader::Mesh* p_tile_mesh, TileVertexBuffer** pp_vertex_buffer_output)
{
	/*
		TODO: Change vertex buffer to use index buffer
	*/

	std::vector<XMFLOAT3>* p_mesh_vertices = p_tile_mesh->GetVertexPositionVector();
	std::vector<int>* p_mesh_indices = p_tile_mesh->GetIndexVector();
	TileVertexBuffer* p_vertex_buffer = new TileVertexBuffer;

	// Fetch all existing tile entities
	TypeFilter tile_filter;
	tile_filter.addRequirement(components::TileComponent::typeID);
	tile_filter.addRequirement(components::ColorComponent::typeID);
	tile_filter.addRequirement(components::TransformComponent::typeID);
	EntityIterator tile_iterator = rEcs.getEntititesByFilter(tile_filter);

	// Calculate total tile count (world tiles + ocean tiles)
	const UINT total_tile_count = (unsigned int)tile_iterator.entities.size() + OCEAN_TILE_COUNT;
	p_vertex_buffer->vertexCount = total_tile_count;
	UINT inited_tile_count = 0;

	// Create vertex array
	p_vertex_buffer->first = new WorldVertex[total_tile_count * p_mesh_indices->size()];

	/*
		Init all world tiles
	*/
	XMFLOAT3 position;
	XMVECTOR xm_position;
	XMMATRIX xm_world;
	components::ColorComponent* p_color;
	components::TransformComponent* p_transform;
	for (FilteredEntity& tile : tile_iterator.entities)
	{
		p_color = tile.getComponent<components::ColorComponent>();
		p_transform = tile.getComponent<components::TransformComponent>();
		xm_world = XMMatrixTranslation(p_transform->position.x, p_transform->position.y, p_transform->position.z);

		//for (int index : *p_mesh_indices)
		//{
		//	xm_position = 
		//	XMStoreFloat3(&position, XMLoadFloat3(&(*mesh_vertices)[index]))
		//	vertex_buffer->first[inited_tile_count++] = WorldVertex()
		//}
	}
}