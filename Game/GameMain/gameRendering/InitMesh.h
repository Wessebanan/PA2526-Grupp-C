#pragma once

#include "rendering/RenderManager.h"
#include "Mesh.h"
#include "ecs.h"



void InitMesh(ecs::EntityComponentSystem& rECS, rendering::RenderManager& mng)
{
	using namespace rendering;
	using namespace DirectX;


	//ModelLoader::Mesh tile("../hexagon_tile.fbx");
	//ModelLoader::Mesh tile2("../hexagon_tile2.fbx");
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
	MODEL_LAYOUT_DESC m_desc[14];
	// first 2 are tile and dude, then 12 sceneobjects

	ecs::ComponentIterator itt2;
	itt2 = rECS.getAllComponentsOfType(ecs::components::SceneObjectComponent::typeID);
	ecs::components::SceneObjectComponent* scene_comp;
	int index = 0;
	ecs::BaseComponent* p_base;
	while (p_base = itt2.next())
	{
		scene_comp = (ecs::components::SceneObjectComponent*)p_base;

		ModelLoader::Mesh mesh(scene_comp->GetFilepath());

		using namespace rendering;
		int mesh_index;
		{
			VERTEX_BUFFER_DATA vertex_data = { NULL };
			vertex_data.VertexCount = mesh.GetVertexPositionVector()->size();

			vertex_data.pVertexData = mesh.GetVertexPositionVector()->data();
			vertex_data.pTextureCoordData = mesh.GetUVVector()->data();

			INDEX_BUFFER_DATA index_data = { NULL };
			index_data.IndexCount = mesh.GetIndexVector()->size() * 4;
			index_data.pIndexData = mesh.GetIndexVector()->data();

			mesh_index = mng.CreateMesh(
				&vertex_data,
				&index_data);
		}


		m_desc[index+2].InstanceCount = 1;
		m_desc[index +2].MeshIndex = mesh_index;

		desc[RENDER_DEFAULT].PerInstanceByteWidth = sizeof(float4);
		desc[RENDER_DEFAULT].pModelLayout = &m_desc[index +2];
		desc[RENDER_DEFAULT].ModelLayoutCount = 1;

		index++;
	}
	




	m_desc[0].InstanceCount = 12*12;
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

}