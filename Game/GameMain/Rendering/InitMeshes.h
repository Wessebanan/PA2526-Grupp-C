#pragma once

#include "rendering/RenderManager.h"
#include "Mesh.h"
#include "..//AI/GridFunctions.h"
#include "..//Utility/UtilityComponents.h"

#include "ecs.h"

using namespace rendering;
using namespace DirectX;

void InitMeshes(ecs::EntityComponentSystem& rECS, RenderManager& mng)
{
	ModelLoader::Mesh tile("../hexagon_tile.fbx");
	ModelLoader::Mesh tile2("../hexagon_tile2.fbx");
	ModelLoader::Mesh tile3("../hexagon_tile3.fbx");
	ModelLoader::Mesh dude("../dudeMesh0.fbx");

	int mesh_tile; // tile
	{
		VERTEX_BUFFER_DATA vertex_data = { NULL };
		vertex_data.VertexCount = tile3.GetVertexPositionVector()->size();
		vertex_data.pVertexData = tile3.GetVertexPositionVector()->data();

		INDEX_BUFFER_DATA index_data = { NULL };
		index_data.IndexCount = tile3.GetIndexVector()->size() * 4;
		index_data.pIndexData = tile3.GetIndexVector()->data();

 		mesh_tile = mng.CreateMesh(
			&vertex_data,
			&index_data);
	}

	int mesh_dude; // dude
	{
		VERTEX_BUFFER_DATA vertex_data = { NULL };
		vertex_data.VertexCount = dude.GetVertexPositionVector()->size();

		vertex_data.pVertexData = dude.GetVertexPositionVector()->data();
		vertex_data.pTextureCoordData = dude.GetUVVector()->data();

		INDEX_BUFFER_DATA index_data = { NULL };
		index_data.IndexCount = dude.GetIndexVector()->size() * 4;
		index_data.pIndexData = dude.GetIndexVector()->data();

		mesh_dude = mng.CreateMesh(
			&vertex_data,
			&index_data);
	}


	struct float4
	{
		float x, y, z;
		uint32_t color;
	};



	TECHNIQUE_HEAP_LAYOUT_DESC desc[RENDER_TECHNIQUES_COUNT] = { 0 };

	// Default Technique will render 'count' meshes in white
	MODEL_LAYOUT_DESC m_desc[2];
	m_desc[0].InstanceCount = 12 * 12; // tile grid
	m_desc[0].MeshIndex = mesh_tile;

	m_desc[1].InstanceCount = 12;
	m_desc[1].MeshIndex = mesh_dude;

	desc[RENDER_DEFAULT].PerInstanceByteWidth = sizeof(float4);
	desc[RENDER_DEFAULT].pModelLayout = &m_desc[0];
	desc[RENDER_DEFAULT].ModelLayoutCount = 1;

	desc[RENDER_TRANSFORMATION].PerInstanceByteWidth = sizeof(float4) * 4;
	desc[RENDER_TRANSFORMATION].pModelLayout = &m_desc[1];
	desc[RENDER_TRANSFORMATION].ModelLayoutCount = 1;

	mng.CreateModelHeap(desc);

	float4* pTilePosition = (float4*)mng.GetTechniqueModelBuffer(RENDER_DEFAULT);
	XMFLOAT4X4* p_unit_pos = (XMFLOAT4X4*)mng.GetTechniqueModelBuffer(RENDER_TRANSFORMATION);


	//Place tiles by TransformeComponents
	UINT index = 0;
	ecs::ComponentIterator itt = rECS.getAllComponentsOfType(TileComponent::typeID);
	ecs::components::TileComponent* tileComp;
	while (tileComp = (ecs::components::TileComponent*)itt.next())
	{
		ecs::components::TransformComponent* trComp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(tileComp->getEntityID());

		pTilePosition[index].x = trComp->position.x;
		pTilePosition[index].y = trComp->position.y;
		pTilePosition[index].z = trComp->position.z;

		int random = rand() % 101;
		int color_offset = -50 + random;
		switch (tileComp->tileType)
		{
		case TileTypes::GRASS:
			pTilePosition[index].color = PACK(0, 150 + color_offset, 0, 0);
			break;
		case TileTypes::STONE:
			pTilePosition[index].color = PACK(50 + color_offset, 50 + color_offset, 50 + color_offset, 0);
			break;
		case TileTypes::WATER:
			pTilePosition[index].color = PACK(0, 0, 200 + color_offset, 0);
			break;
		case TileTypes::UNDEFINED:
			pTilePosition[index].color = PACK(0, 0, 0, 255);
			break;
		default:
			pTilePosition[index].color = PACK(255, 255, 255, 255);
			break;
		}

		index++;
	}


	// Place army depending on transform compnents
	int armyIndex = 0;
	itt = rECS.getAllComponentsOfType(ecs::components::ArmyComponent::typeID);
	ecs::components::ArmyComponent* armComp;
	while (armComp = (ecs::components::ArmyComponent*)itt.next())
	{
		// over alla units in the army
		for (size_t i = 0; i < 3; i++)
		{
			ecs::components::TransformComponent* trComp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(armComp->unitIDs[i]);

			XMMATRIX world = XMMatrixIdentity();
			world *= XMMatrixScaling(trComp->scale.x, trComp->scale.y, trComp->scale.z);
			world *= XMMatrixRotationRollPitchYaw(trComp->rotation.x, trComp->rotation.y, trComp->rotation.z);
			world *= XMMatrixTranslation(trComp->position.x, trComp->position.y, trComp->position.z);


			XMStoreFloat4x4(&p_unit_pos[armyIndex], world);

			armyIndex++;
		}
	}
}