
#include "Input/InitInput.h"
#include "../../Graphics/includes/Window.h"
#include "ecs.h"



//#include "gameRendering/InitMesh.h"
//#include "gameRendering/PlaceMesh.h"

#include "gameAI/InitArmy.h"
#include "gameAI/InitGrid.h"

#include "Input/InitInterpreter.h"

#include "gameUtility/InitCamera.h"

#include "gameSceneObjects/InitSceneObjectsh.h"

#include "gameAudio/InitAudio.h"

#include "Physics/InitPhysics.h"

#include "gameSceneObjects/InitOcean.h"
#include "gameWorld/InitWorld.h"

#include "Mesh.h"


#include "../../Graphics/includes/RenderManager.h"
#include "../../Graphics/includes/MeshManager.h"

#include "gameGraphics/ForwardRenderingPipeline.h"
#include "gameGraphics/ShadowMapPipeline.h"

//#include "../../Graphics/includes/ForwardRenderingPipeline.h"

#include <time.h>

inline uint32_t PACK(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3) {
	return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
}

//using namespace ecs;				

void SetViewMatrix(
	DirectX::XMFLOAT4X4& rViewMatrix,
	const float x,
	const float y,
	const float z,
	const float dx,
	const float dy,
	const float dz)
{
	DirectX::XMStoreFloat4x4(&rViewMatrix,
		DirectX::XMMatrixLookToLH(
			{ x, y, z },
			{ dx, dy,  dz },
			{ 0.0f, 1.0f,  0.0f }
	));
}

const std::string GetShaderFilepath(const char* pFilename)
{
	std::string filepath = "..//";

#ifdef _DEBUG
	filepath.append("shaders_d//");
#else
	filepath.append("shaders//");
#endif // _DEBUG

	filepath.append(pFilename);

	return filepath;
}


graphics::MeshRegion UploadMeshToGPU(ModelLoader::Mesh& mesh, graphics::MeshManager& rMng)
{
	graphics::MeshRegion mesh_region;

	{
		mesh_region = rMng.CreateMeshRegion(
			mesh.GetVertexPositionVector()->size(),
			mesh.GetIndexVector()->size());

		{
			graphics::VERTEX_DATA data = { NULL };
			data.pVertexPositions	= mesh.GetVertexPositionVector()->data();
			data.pVertexNormals		= mesh.GetNormalVector()->data();
			data.pVertexTexCoords	= mesh.GetUVVector()->data();

			rMng.UploadData(
				mesh_region,
				data,
				mesh.GetIndexVector()->data());
		}
	}

	return mesh_region;
}


int main()
{
	srand(time(0));


	/*
		-- Client Resolution --
	*/
	const UINT
		client_width = 1920,
		client_height = 1080;

	graphics::Window wnd;
	wnd.Initialize(
		client_width,
		client_height,
		"Couch Commanders",
		graphics::WINDOW_STYLE::BORDERLESS);

	graphics::RenderManager renderer;
	renderer.Initialize(wnd);

	graphics::MeshManager mesh_manager;
	mesh_manager.Initialize(500000, 500000);


	/*
		-- Meshes --
	*/
	ModelLoader::Mesh mesh_hexagon("../meshes/hexagon_tile5.fbx");
	ModelLoader::Mesh mesh_rock("../meshes/rock.fbx");
	ModelLoader::Mesh mesh_tree("../meshes/tree.fbx");


	//graphics::MeshRegion mesh_region_hexagon0 = UploadMeshToGPU(mesh_hexagon, mesh_manager);
	graphics::MeshRegion mesh_region_tree = UploadMeshToGPU(mesh_tree, mesh_manager);
	graphics::MeshRegion mesh_region_rock = UploadMeshToGPU(mesh_rock, mesh_manager);


	UINT pipeline_shadow_map;
	{
		graphics::SHADOW_MAP_PIPELINE_DESC desc;
		desc.PixelsWidth	= 1024;
		desc.Width			= 25.0f;
		desc.Height			= 40.0f;
		desc.NearPlane		=  1.0f;
		desc.FarPlane		= 40.0f;
		pipeline_shadow_map = renderer.CreatePipeline(
			new graphics::ShadowMapPipeline,
			&desc);
	}

	UINT pipeline_forward;
	{
		graphics::FORWARD_RENDERING_PIPELINE_DESC desc;
		desc.ClientWidth	= client_width;
		desc.ClientHeight	= client_height;
		desc.Fov			= 3.14f / 2.0f;
		desc.NearPlane		= 0.1f;
		desc.FarPlane		= 100.0f;
		pipeline_forward = renderer.CreatePipeline(
			new graphics::ForwardRenderingPipeline,
			&desc);
	}

	const std::string vs = GetShaderFilepath("VS_Default.cso");
	const std::string ps = GetShaderFilepath("PS_Default.cso");
	UINT shader_index0 = renderer.CreateShaderProgram(
		vs.c_str(), 
		ps.c_str(), 
		sizeof(float) * 3 + sizeof(UINT));


	ecs::EntityComponentSystem ecs;

	const unsigned int LOGIC_TILE_COUNT = 144;
	const unsigned int SCENE_OBJECT_COUNT = 12;
	const unsigned int UNIT_COUNT = 12;
	const unsigned int CAMERA_COUNT = 1;

	const unsigned int COMPONENT_TRANSFORM_COUNT = LOGIC_TILE_COUNT + OCEAN_TILE_COUNT + SCENE_OBJECT_COUNT + UNIT_COUNT + CAMERA_COUNT;
	const unsigned int COMPONENT_COLOR_COUNT = LOGIC_TILE_COUNT + OCEAN_TILE_COUNT + SCENE_OBJECT_COUNT + UNIT_COUNT;

	// Tiles + sceneobjects + units + camera
	ecs.reserveComponentCount<ecs::components::TransformComponent>(COMPONENT_TRANSFORM_COUNT);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(COMPONENT_COLOR_COUNT);

	ecs.reserveComponentCount<ecs::components::TileComponent>(LOGIC_TILE_COUNT);
	ecs.reserveComponentCount<ecs::components::OceanTileComponent>(OCEAN_TILE_COUNT);


	//InitSound(ecs);

	InitInput(ecs);
	InitInterpreter(ecs);

	InitGrid(ecs);
	InitArmy(ecs);
	InitSceneObjects(ecs);

	InitCamera(ecs);

	//InitPhysics(ecs, pp_meshes);

	// to get components in the loop
	ecs::ComponentIterator itt;

	itt = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	ecs::components::CameraComponent* p_cam_comp = (ecs::components::CameraComponent*)itt.next();






	// ##################################################################################### //
	// ############################## MACKE GREJJER START ¤################################# //
	// ##################################################################################### //
	InitOcean(ecs);

	//WorldVertex triangle[] = {
	//	{ XMFLOAT3(-50.f, 0.f, -50.f) },
	//	{ XMFLOAT3(0.f, 0.f, 50.f) },
	//	{ XMFLOAT3(50.f, 0.f, -50.f)}
	//};

	VertexColor* colors;
	TileVertexBuffer* p_world_vertex_buffer = nullptr;
	CreateWorldTileVertexBuffer(ecs, &mesh_hexagon, &p_world_vertex_buffer, &colors);

	graphics::MeshRegion world_mesh_region = mesh_manager.CreateMeshRegion(p_world_vertex_buffer->vertexCount, 0);
	graphics::VERTEX_DATA world_vertex_data = { NULL };
	world_vertex_data.pVertexPositions = p_world_vertex_buffer->pFirst;
	mesh_manager.UploadData(world_mesh_region, world_vertex_data, nullptr);

	//graphics::MeshRegion world_mesh_region = mesh_manager.CreateMeshRegion(3, 0);
	//graphics::VERTEX_DATA world_vertex_data;
	//world_vertex_data.pVertexPositions = triangle;
	//mesh_manager.UploadData(world_mesh_region, world_vertex_data, nullptr);


	// ##################################################################################### //
	// ############################## MACKE GREJJER END ¤################################### //
	// ##################################################################################### //

	graphics::ShaderModelLayout layout;
	layout.MeshCount			= 3;
	//layout.Meshes[0]			= (mesh_region_hexagon);
	layout.Meshes[0]			= (world_mesh_region); // Macke
	layout.Meshes[1]			= (mesh_region_tree);
	layout.Meshes[2]			= (mesh_region_rock);

	struct ShaderProgramInput
	{
		float x, y, z;
		uint32_t Color;
	};

	UINT index = 0;
	itt = ecs.getAllComponentsOfType(ecs::components::TileComponent::typeID);
	ecs::components::TileComponent* tileComp;
	//size_t size = ecs.getComponentCountOfType(ecs::components::TileComponent::typeID);
	size_t size = 1;

	//ShaderProgramInput* pData = new ShaderProgramInput[size * 3];
	ShaderProgramInput* pData = new ShaderProgramInput[size + 12]; // Macke
	pData[0] = { 0 };
	pData[0].Color = PACK(30, 255, 64, 255);
	//layout.InstanceCounts[0] = size;
	layout.InstanceCounts[0] = 1; // Macke
	layout.InstanceCounts[1] = 6;
	layout.InstanceCounts[2] = 6;

	//ZeroMemory(pData, sizeof(ShaderProgramInput) * size);

	for (UINT i = 0; i < layout.MeshCount; i++)
	{
		layout.TotalModels = layout.InstanceCounts[i];
	}

	//while (tileComp = (ecs::components::TileComponent*)itt.next())
	//{
	//	ecs::components::TransformComponent* trComp = ecs.getComponentFromEntity<ecs::components::TransformComponent>(tileComp->getEntityID());
	//	ecs::components::ColorComponent* color_comp = ecs.getComponentFromEntity<ecs::components::ColorComponent>(tileComp->getEntityID());

	//	pData[index].x = trComp->position.x;
	//	pData[index].y = trComp->position.y;
	//	pData[index].z = trComp->position.z;

	//	int random = rand() % 101;
	//	int color_offset = -50 + random;
	//	switch (tileComp->tileType)
	//	{
	//	case TileTypes::GAME_FIELD:
	//		pData[index].Color = PACK(color_comp->red, color_comp->green, color_comp->blue, 0);
	//		break;
	//	case TileTypes::WATER:
	//		pData[index].Color = PACK(0, 0, 200 + color_offset, 0);
	//		break;
	//	case TileTypes::UNDEFINED:
	//		pData[index].Color = PACK(0, 0, 0, 255);
	//		break;
	//	default:
	//		pData[index].Color = PACK(255, 255, 255, 255);
	//		break;
	//	}

	//	index++;
	//}

	UINT scene_objects_index = size;
	itt = ecs.getAllComponentsOfType(ecs::components::SceneObjectComponent::typeID);
	ecs::components::SceneObjectComponent* scene_comp;
	while (scene_comp = (ecs::components::SceneObjectComponent*)itt.next())
	{
		ecs::components::TransformComponent* trComp = ecs.getComponentFromEntity<ecs::components::TransformComponent>(scene_comp->getEntityID());
		ecs::components::ColorComponent* color_comp = ecs.getComponentFromEntity<ecs::components::ColorComponent>(scene_comp->getEntityID());

		pData[scene_objects_index].x = trComp->position.x;
		pData[scene_objects_index].y = trComp->position.y;
		pData[scene_objects_index].z = trComp->position.z;

	

	//graphics::PresentWindow* pWnd = pMng->GetPresentWindow();

		pData[scene_objects_index].Color = PACK(color_comp->red, color_comp->green, color_comp->blue, 0);


		scene_objects_index++;
	}

	renderer.SetShaderModelLayout(shader_index0, layout);
	//renderer.SetModelData(pData, sizeof(ShaderProgramInput) * size * 3);
	renderer.SetModelData(pData, sizeof(ShaderProgramInput) * (size + 12)); // Macke

	wnd.Open();
	while (wnd.IsOpen())
	{
		if (!wnd.Update()) 
		{
			if (GetAsyncKeyState(VK_ESCAPE))
			{
				wnd.Close();
			}

			ecs.update(0.1f);

			{	
				DirectX::XMFLOAT4X4 shadow_matrix;
				SetViewMatrix(
					shadow_matrix,
					6.0f,  4.0f, -5.0f,
					0.0f, -1.0f, 1.0f);

				graphics::SHADOW_MAP_PIPELINE_DATA data;
				data.ViewMatrix = shadow_matrix;

				renderer.UpdatePipeline(pipeline_shadow_map, &data);
			}

			{
				graphics::FORWARD_RENDERING_PIPELINE_DATA data;
				data.ViewMatrix = p_cam_comp->viewMatrix;
				data.Red	= 0.25f;
				data.Green	= 0.25f;
				data.Blue	= 1.0f;

				renderer.UpdatePipeline(pipeline_forward, &data);
			}

			renderer.ExecutePipeline(pipeline_shadow_map);
			renderer.ExecutePipeline(pipeline_forward);

			renderer.Present();
		}
	}
	
	//for (int i = 0; i < Mesh::N_MESHES; i++) delete pp_meshes[i];
	//delete[] pp_meshes;

	mesh_manager.Destroy();
	renderer.Destroy();
	return 0;
}