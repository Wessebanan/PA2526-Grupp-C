
#include "Input/InitInput.h"
#include "../../Graphics/includes/Window.h"
#include "ecs.h"



//#include "gameRendering/InitMesh.h"
//#include "gameRendering/PlaceMesh.h"

#include "gameAI/InitArmy.h"
#include "gameAI/InitGrid.h"
#include "gameAI/InitAI.h"
#include "AIGlobals.h"

#include "Input/InitInterpreter.h"

#include "gameUtility/InitCamera.h"

#include "gameSceneObjects/InitSceneObjectsh.h"
#include "gameSceneObjects/InitBiomes.h"
#include "gameUtility/UtilityEcsFunctions.h"

#include "gameAudio/InitAudio.h"

#include "Physics/InitPhysics.h"

#include "Mesh.h"


#include "../../Graphics/includes/RenderManager.h"
#include "../../Graphics/includes/MeshManager.h"

#include "gameGraphics/ForwardRenderingPipeline.h"
#include "gameGraphics/ShadowMapPipeline.h"

#include "gameAnimation/InitAnimation.h"

#include "MeshContainer/MeshContainer.h"

#include "Renderers/Renderers.h"

//#include "../../Graphics/includes/ForwardRenderingPipeline.h"

#include <time.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

/*inline uint32_t PACK(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3)
{
	return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
}	*/			



void InitAll(EntityComponentSystem& rECS);

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

//const std::string GetShaderFilepath(const char* pFilename)
//{
//	std::string filepath = "..//";
//
//#ifdef _DEBUG
//	filepath.append("shaders_d//");
//#else
//	filepath.append("shaders//");
//#endif // _DEBUG
//
//	//filepath.append("shaders_d//");
//
//	filepath.append(pFilename);
//
//	return filepath;
//}


//graphics::MeshRegion UploadMeshToGPU(ModelLoader::Mesh& mesh, graphics::MeshManager& rMng)
//{
//	graphics::MeshRegion mesh_region;
//
//	{
//		mesh_region = rMng.CreateMeshRegion(
//			mesh.GetVertexPositionVector()->size(),
//			mesh.GetIndexVector()->size());
//
//		{
//			graphics::VERTEX_DATA data = { NULL };
//			data.pVertexPositions		= mesh.GetVertexPositionVector()->data();
//			data.pVertexNormals			= mesh.GetNormalVector()->data();
//			data.pVertexTexCoords		= mesh.GetUVVector()->data();
//			if (mesh.HasSkeleton())
//			{
//				data.pVertexBlendWeights = mesh.GetBlendWeights()->data();
//				data.pVertexBlendIndices = mesh.GetBlendIndices()->data();
//			}
//
//
//			rMng.UploadData(
//				mesh_region,
//				data,
//				mesh.GetIndexVector()->data());
//		}
//	}
//
//	return mesh_region;
//}


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	const UINT RENDER_BUFFER_SIZE = PAD(pow(10, 6), 256);

	srand(time(0));

	/*
		-- Client Resolution --
	*/
	const UINT
		client_width = graphics::GetDisplayResolution().x,
		client_height = graphics::GetDisplayResolution().y;

	graphics::Window wnd;
	wnd.Initialize(
		client_width,
		client_height,
		"Couch Commanders",
		graphics::WINDOW_STYLE::BORDERLESS);

	/*
		Initialize graphics
	*/

	graphics::InitializeD3D11();
	graphics::AttachHwndToSwapChain(wnd);

	graphics::RenderManager renderer;
	renderer.Initialize(RENDER_BUFFER_SIZE);

	graphics::MeshManager mesh_manager;
	mesh_manager.Initialize(10000, 10000);

	// Will be moved to a system
	UINT pipeline_shadow_map;
	{
		graphics::SHADOW_MAP_PIPELINE_DESC desc;
		desc.PixelsWidth = 1024;
		desc.Width = 25.0f;
		desc.Height = 40.0f;
		desc.NearPlane = 1.0f;
		desc.FarPlane = 40.0f;
		pipeline_shadow_map = renderer.CreatePipeline(
			new graphics::ShadowMapPipeline,
			&desc);
	}

	// Will be moved to a system
	UINT pipeline_forward;
	{
		graphics::FORWARD_RENDERING_PIPELINE_DESC desc;
		desc.ClientWidth = client_width;
		desc.ClientHeight = client_height;
		desc.Fov = 3.14f / 2.0f;
		desc.NearPlane = 0.1f;
		desc.FarPlane = 100.0f;
		pipeline_forward = renderer.CreatePipeline(
			new graphics::ForwardRenderingPipeline,
			&desc);
	}


	/*
		-- Meshes --
	*/

	MeshContainer::Initialize(&mesh_manager);
	MeshContainer::LoadMesh(MESH_TYPE_TILE, "../meshes/hexagon_tile5.fbx");
	MeshContainer::LoadMesh(MESH_TYPE_ROCK, "../meshes/rock.fbx");
	MeshContainer::LoadMesh(MESH_TYPE_TREE, "../meshes/tree2.fbx");
	MeshContainer::LoadMesh(MESH_TYPE_UNIT, "../RunningCustom2.fbx");

	/* ECS BEGIN */

	ecs::EntityComponentSystem ecs;

	//Tiles + sceneobjects + units + camera
	ecs.reserveComponentCount<ecs::components::TransformComponent>(4000);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(4000);
	ecs.reserveComponentCount<ecs::components::TileComponent>(4000);

	InitAll(ecs);

	// to get components in the loop
	ecs::components::CameraComponent* p_cam_comp = (components::CameraComponent*)ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID).next();

	/* ECS END */



	/*
		Set GPU buffer
	*/

	graphics::RenderBuffer renderBuffer;
	renderBuffer.Initialize(RENDER_BUFFER_SIZE, 256);

	systems::UnitRenderSystem* p_unit_renderer = ecs.createSystem<systems::UnitRenderSystem>(9);
	systems::SceneObjectRenderSystem* p_scenery_renderer = ecs.createSystem<systems::SceneObjectRenderSystem>(9);
	systems::TileRenderSystem* p_tile_renderer = ecs.createSystem<systems::TileRenderSystem>(9);

	p_unit_renderer->Initialize(&renderer, &renderBuffer);
	p_scenery_renderer->Initialize(&renderer, &renderBuffer);
	p_tile_renderer->Initialize(&renderer, &renderBuffer);
	
	unsigned long long int frame_count = 0;
	unsigned long long int frame_count2 = 0;
	wnd.Open();

	while (wnd.IsOpen())
	{
		if (!wnd.Update())
		{
			if (GetAsyncKeyState(VK_ESCAPE))
			{
				wnd.Close();
			}

			renderBuffer.Reset();

			ecs.update(0.002f);

			renderer.BeginUpload();
			renderer.UploadPerInstanceData(renderBuffer.GetStartAddress(), renderBuffer.GetUsedMemory(), 0);

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

			mesh_manager.SetVertexBuffers();

			if(frame_count % 2 == 0)
				renderer.ExecutePipeline(pipeline_shadow_map);

			renderer.ExecutePipeline(pipeline_forward);

			graphics::Present(0);

			frame_count++;
			if (frame_count % 3 == 0)
			{
				frame_count2++;
			}
		}
	}

	mesh_manager.Destroy();
	renderer.Destroy();

	graphics::DestroyD3D11();

	MeshContainer::Terminate();
	renderBuffer.Terminate();

	return 0;

}

void InitAll(EntityComponentSystem& rECS)
{
	InitSound(rECS);

	InitAI(rECS);

	InitInput(rECS);
	InitInterpreter(rECS);

	InitGrid(rECS);
	InitArmy(rECS);
	InitSceneObjects(rECS);

	InitCamera(rECS);

	InitPhysics(rECS, MeshContainer::GetMeshCPU(MESH_TYPE_UNIT));
}