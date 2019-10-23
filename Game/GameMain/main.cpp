
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

#include "gameUtility/Timer.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

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
		-- Initialize Graphics --
	*/

	graphics::InitializeD3D11();
	graphics::AttachHwndToSwapChain(wnd);

	graphics::RenderManager renderer;
	renderer.Initialize(RENDER_BUFFER_SIZE);

	graphics::MeshManager mesh_manager;
	mesh_manager.Initialize(10000, 10000);

	UINT pipeline_shadow_map;
	{
		graphics::SHADOW_MAP_PIPELINE_DESC desc;
		desc.PixelsWidth	= 2048;
		desc.Width			= 45.0f;
		desc.Height			= 80.0f;
		desc.NearPlane		=  1.0f;
		desc.FarPlane		= 40.0f;
		pipeline_shadow_map = renderer.CreatePipeline(
			new graphics::ShadowMapPipeline,
			&desc);
	}

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
		-- Load Meshes --
		
		MeshContainer is a singleton, and is visible for everyone.
	*/

	MeshContainer::Initialize(&mesh_manager);
	MeshContainer::LoadMesh(MESH_TYPE_TILE, "../meshes/hexagon_tile5.fbx");
	MeshContainer::LoadMesh(MESH_TYPE_ROCK, "../meshes/rock.fbx");
	MeshContainer::LoadMesh(MESH_TYPE_TREE, "../meshes/tree2.fbx");
	MeshContainer::LoadMesh(MESH_TYPE_UNIT, "../RunningCustom2.fbx");

	/*
		-- Initialize ECS --
	*/

	ecs::EntityComponentSystem ecs;

	//Tiles + sceneobjects + units + camera
	ecs.reserveComponentCount<ecs::components::TransformComponent>(5000);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(5000);
	ecs.reserveComponentCount<ecs::components::TileComponent>(5000);

	/*
		InitAll is a list of ecs system Init-functions.
		If you with to add your own Init-function, please add in
		in InitAll().
	*/

	InitAll(ecs);


	/*
		-- Initialize Mesh Renderers --
	*/

	graphics::RenderBuffer renderBuffer;
	renderBuffer.Initialize(RENDER_BUFFER_SIZE, 256);

	systems::UnitRenderSystem* p_unit_renderer = ecs.createSystem<systems::UnitRenderSystem>(9);
	systems::SceneObjectRenderSystem* p_scenery_renderer = ecs.createSystem<systems::SceneObjectRenderSystem>(9);
	systems::TileRenderSystem* p_tile_renderer = ecs.createSystem<systems::TileRenderSystem>(9);

	p_unit_renderer->Initialize(&renderer, &renderBuffer);
	p_scenery_renderer->Initialize(&renderer, &renderBuffer);
	p_tile_renderer->Initialize(&renderer, &renderBuffer);




	/*
		 #############################                                                    ############################# 
		#######################   From here on, all initialization is expected to be finished.   #######################
		 #############################                                                    ############################# 
	*/




	/*
		-- Show Window --
	*/

	wnd.Open();

	/*
		-- Create Timer for update loop --
	*/

	Timer timer;
	timer.StartGame();

	/*
		-- Update Loop, while window is open --
	*/

	while (wnd.IsOpen())
	{
		if (!wnd.Update())
		{ 
			// Close window when user press Esc
			if (GetAsyncKeyState(VK_ESCAPE))
			{
				wnd.Close();
			}

			/*
				Clear all data from render buffer. This buffer will be filled each frame with
				per vertex data for all objects that will render, and then be uploaded to the GPU
				all together.
			*/
			renderBuffer.Reset();

			/*
				Update all ECS systems, and give them the delta time.
			*/
			ecs.update(timer.GetFrameTime());

			/*
				Upload renderBuffer to the GPU.
				RenderBuffer contains ALL vertex data for rendering.
			*/
			renderer.BeginUpload();
			renderer.UploadPerInstanceData(renderBuffer.GetStartAddress(), renderBuffer.GetUsedMemory(), 0);

			/*
				Update shadow map for graphics engine
			*/
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

			/*
				Update camera for graphics engine
			*/
			{
				CameraComponent* p_cam_comp = (CameraComponent*)ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID).next();
				graphics::FORWARD_RENDERING_PIPELINE_DATA data;
				data.ViewMatrix = p_cam_comp->viewMatrix;
				data.Red	= 0.25f;
				data.Green	= 0.25f;
				data.Blue	= 1.0f;

				renderer.UpdatePipeline(pipeline_forward, &data);
			}

			mesh_manager.SetVertexBuffers();

			renderer.ExecutePipeline(pipeline_shadow_map);
			renderer.ExecutePipeline(pipeline_forward);

			graphics::Present(0);
		}
	}

	/*
		-- Cleanup Memory --
	*/

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
	InitSong(rECS);

	InitAI(rECS);

	InitInput(rECS);
	InitInterpreter(rECS);

	InitGrid(rECS);
	InitArmy(rECS);
	InitSceneObjects(rECS);

	InitCamera(rECS);

	InitPhysics(rECS, MeshContainer::GetMeshCPU(MESH_TYPE_UNIT));

	ChangeUserStateEvent e;
	e.newState = ATTACK;
	e.playerId = PLAYER1;
	rECS.createEvent(e);
	e.playerId = PLAYER2;
	rECS.createEvent(e);
	e.playerId = PLAYER3;
	rECS.createEvent(e);
	e.playerId = PLAYER4;
	rECS.createEvent(e);
}