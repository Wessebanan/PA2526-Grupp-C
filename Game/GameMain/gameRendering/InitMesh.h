#pragma once

//#include "rendering/RenderManager.h"

#include "../../Graphics/includes/MeshManager.h"
#include "Mesh.h"
#include "ecs.h"
#include "../gameSceneObjects/SceneObjectComponents.h"
#include "../Physics/InitPhysics.h"
#include "../GraphicsComponents.h"

ModelLoader::Mesh** InitMesh(ecs::EntityComponentSystem& rECS, graphics::MeshManager& rMng)
{
	using namespace rendering;
	using namespace DirectX;

	//ModelLoader::Mesh tile("../hexagon_tile.fbx");
	//ModelLoader::Mesh tile2("../hexagon_tile2.fbx");
	ModelLoader::Mesh tile3("../hexagon.fbx");
	ModelLoader::Mesh dude("../dudeMesh2.fbx");

	ecs::components::ModelComponent mod_comp;

	graphics::MeshRegion mesh_region_tile; // tile
	{
		mesh_region_tile = rMng.CreateMeshRegion(
			tile3.GetVertexPositionVector()->size(),
			tile3.GetIndexVector()->size());

		graphics::VERTEX_DATA vertex_data = { NULL };
		vertex_data.pVertexPositions	= tile3.GetVertexPositionVector()->data();
		vertex_data.pVertexNormals		= tile3.GetNormalVector()->data();
		vertex_data.pVertexTexCoords	= tile3.GetUVVector()->data();

		rMng.UploadData(
			mesh_region_tile,
			vertex_data,
			tile3.GetIndexVector()->data());

		//rECS.createEntity(mod_comp);
	}

	graphics::MeshRegion mesh_region_dude; // dude
	{
		mesh_region_dude = rMng.CreateMeshRegion(
			dude.GetVertexPositionVector()->size(),
			dude.GetIndexVector()->size());

		graphics::VERTEX_DATA vertex_data = { NULL };
		vertex_data.pVertexPositions	= dude.GetVertexPositionVector()->data();
		vertex_data.pVertexNormals		= dude.GetNormalVector()->data();
		vertex_data.pVertexTexCoords	= dude.GetUVVector()->data();

		rMng.UploadData(
			mesh_region_dude,
			vertex_data,
			dude.GetIndexVector()->data());
	}


	struct float4
	{
		float x, y, z;
		uint32_t color;
	};


	//TECHNIQUE_HEAP_LAYOUT_DESC desc[RENDER_TECHNIQUES_COUNT] = { 0 };
	//// Default Technique will render 'count' meshes in white
	//MODEL_LAYOUT_DESC m_desc[14];
	//// first 2 are tile and dude, then 12 sceneobjects


	//m_desc[0].InstanceCount = 12 * 12;
	//m_desc[0].MeshIndex = mesh_tile;

	//ecs::ComponentIterator itt2;
	//itt2 = rECS.getAllComponentsOfType(ecs::components::SceneObjectComponent::typeID);
	//ecs::components::SceneObjectComponent* p_scene_comp;
	//int index = 1;
	//ecs::BaseComponent* p_base;
	//while (p_base = itt2.next())
	//{
	//	p_scene_comp = (ecs::components::SceneObjectComponent*)p_base;

	//	ModelLoader::Mesh mesh(p_scene_comp->GetFilepath());
	//	//ModelLoader::Mesh mesh("gameSceneObjects/models/tree.fbx");

	//	using namespace rendering;
	//	int mesh_index;
	//	{
	//		VERTEX_BUFFER_DATA vertex_data = { NULL };
	//		vertex_data.VertexCount = mesh.GetVertexPositionVector()->size();

	//		vertex_data.pVertexData = mesh.GetVertexPositionVector()->data();
	//		vertex_data.pTextureCoordData = mesh.GetUVVector()->data();

	//		INDEX_BUFFER_DATA index_data = { NULL };
	//		index_data.IndexCount = mesh.GetIndexVector()->size();
	//		index_data.pIndexData = mesh.GetIndexVector()->data();

	//		mesh_index = pMng->CreateMesh(
	//			&vertex_data,
	//			&index_data);
	//	}


	//	m_desc[index].InstanceCount = 1;
	//	m_desc[index].MeshIndex = mesh_index;

	//	index++;
	//}

	//m_desc[13].InstanceCount = 12;
	//m_desc[13].MeshIndex = mesh_dude;


	//desc[RENDER_DEFAULT].PerInstanceByteWidth = sizeof(float4);
	//desc[RENDER_DEFAULT].pModelLayout = &m_desc[0];
	//desc[RENDER_DEFAULT].ModelLayoutCount = 13;

	//desc[RENDER_TRANSFORMATION].PerInstanceByteWidth = sizeof(float4) * 4;
	//desc[RENDER_TRANSFORMATION].pModelLayout = &m_desc[13];
	//desc[RENDER_TRANSFORMATION].ModelLayoutCount = 1;

	//pMng->CreateModelHeap(desc);

	// Physics needs meshes, if this function for some reason is called more than
	// once, the memory is not allocated again.
	static bool init = false;
	if (!init)
	{
		ModelLoader::Mesh** pp_meshes = new ModelLoader::Mesh * [Mesh::N_MESHES];
		ModelLoader::Mesh* p_dude = new ModelLoader::Mesh("../Walking2.fbx");
		ModelLoader::Mesh* p_tile = new ModelLoader::Mesh("../hexagon.fbx");
		pp_meshes[Mesh::DUDE] = p_dude;
		pp_meshes[Mesh::TILE] = p_tile;		
		init = true;
		return pp_meshes;
	}
	else
	{
		return nullptr;
	}
}