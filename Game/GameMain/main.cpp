
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
		desc.PixelsWidth	= 2048;
		desc.Width			= 45.0f;
		desc.Height			= 80.0f;
		desc.NearPlane		=  1.0f;
		desc.FarPlane		= 40.0f;
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
	ecs.reserveComponentCount<ecs::components::TransformComponent>(5000);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(5000);
	ecs.reserveComponentCount<ecs::components::TileComponent>(5000);

	InitSound(ecs);
	InitSong(ecs);
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

	Timer timer;

	timer.StartGame();

	ChangeUserStateEvent e;
	e.newState = ATTACK;
	e.playerId = PLAYER1;
	ecs.createEvent(e);
	e.playerId = PLAYER2;
	ecs.createEvent(e);
	e.playerId = PLAYER3;
	ecs.createEvent(e);
	e.playerId = PLAYER4;
	ecs.createEvent(e);

	while (wnd.IsOpen())
	{
		if (!wnd.Update())
		{ 
			if (GetAsyncKeyState(VK_ESCAPE))
			{
				wnd.Close();
			}
			ecs.update(timer.GetFrameTime());

			ecs::TypeFilter army_filter;
			army_filter.addRequirement(ecs::components::UnitComponent::typeID);
			army_filter.addRequirement(ecs::components::TransformComponent::typeID);
			army_filter.addRequirement(ecs::components::ColorComponent::typeID);

			ecs::EntityIterator ei = ecs.getEntititesByFilter(army_filter);

			int armyIndex = 0;
			ecs::components::ColorComponent* p_color_component;
			ecs::components::TransformComponent* p_transform_component;
			for (ecs::FilteredEntity unit : ei.entities)
			{
				p_transform_component = unit.getComponent<ecs::components::TransformComponent>();
				p_color_component = unit.getComponent<ecs::components::ColorComponent>();

				XMMATRIX world = XMMatrixIdentity();
				world *= XMMatrixScaling(p_transform_component->scale.x, p_transform_component->scale.y, p_transform_component->scale.z);
				world *= XMMatrixRotationRollPitchYaw(p_transform_component->rotation.x, p_transform_component->rotation.y, p_transform_component->rotation.z);
				world *= XMMatrixTranslation(p_transform_component->position.x, p_transform_component->position.y, p_transform_component->position.z);

				// Set World Matrix For a Unit
				XMStoreFloat4x4(&skin_shader_program_input[armyIndex].world, world);

				// Pack Color Into A Homogeneous Coordinate In The Matrix (is that correct?)
				// (this value is always a 1.0f in the matrix)
				/*
						   _ Float4x4 _
					| 1.0f, 0.0f, 0.0f, 0.0f |
					| 0.0f, 1.0f, 0.0f, 0.0f |
					| 0.0f, 0.0f, 1.0f, 0.0f |
					| 0.0f, 0.0f, 0.0f, 1.0f | <- this number (1.0f) is used for color
					
					color :
						8bit - red
						8bit - green
						8bit - blue
						8bit - other (not used)
				*/

				skin_shader_program_input[armyIndex].world._44 = PACK(
					p_color_component->red, 
					p_color_component->green, 
					p_color_component->blue,
					0);

				armyIndex++;
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