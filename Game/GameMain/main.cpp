
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

inline uint32_t PACK(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3)
{
	return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
}				

inline uint32_t PAD(const uint32_t size, const uint32_t alignment)
{
	//return size + (alignment - (size % alignment));
	return (uint32_t) (ceil(size / (float)alignment) * alignment);
}

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

const std::string GetShaderFilepath(const char* pFilename)
{
	std::string filepath = "..//";

#ifdef _DEBUG
	filepath.append("shaders_d//");
#else
	filepath.append("shaders//");
#endif // _DEBUG

	//filepath.append("shaders_d//");

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
			data.pVertexPositions		= mesh.GetVertexPositionVector()->data();
			data.pVertexNormals			= mesh.GetNormalVector()->data();
			data.pVertexTexCoords		= mesh.GetUVVector()->data();
			if (mesh.HasSkeleton())
			{
				data.pVertexBlendWeights = mesh.GetBlendWeights()->data();
				data.pVertexBlendIndices = mesh.GetBlendIndices()->data();
			}


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
		client_width = graphics::GetDisplayResolution().x,
		client_height = graphics::GetDisplayResolution().y;

	graphics::Window wnd;
	wnd.Initialize(
		client_width,
		client_height,
		"Couch Commanders",
		graphics::WINDOW_STYLE::BORDERLESS);

	graphics::InitializeD3D11();
	graphics::AttachHwndToSwapChain(wnd);

	graphics::RenderManager renderer;
	renderer.Initialize(131072);

	graphics::MeshManager mesh_manager;
	mesh_manager.Initialize(10000, 10000);


	/*
		-- Meshes --
	*/

	MeshContainer::Initialize(&mesh_manager);

	MeshContainer::LoadMesh(MESH_TYPE_TILE, "../meshes/hexagon_tile5.fbx");
	MeshContainer::LoadMesh(MESH_TYPE_ROCK, "../meshes/rock.fbx");
	MeshContainer::LoadMesh(MESH_TYPE_TREE, "../meshes/tree2.fbx");
	MeshContainer::LoadMesh(MESH_TYPE_UNIT, "../RunningCustom2.fbx");

	graphics::MeshRegion mesh_region_hexagon	= MeshContainer::GetMeshGPU(MESH_TYPE_TILE);
	graphics::MeshRegion mesh_region_tree		= MeshContainer::GetMeshGPU(MESH_TYPE_TREE);
	graphics::MeshRegion mesh_region_rock		= MeshContainer::GetMeshGPU(MESH_TYPE_ROCK);
	graphics::MeshRegion mesh_region_dude		= MeshContainer::GetMeshGPU(MESH_TYPE_UNIT);

	// Will be moved to a system
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

	// Will be moved to a system
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
	const std::string vs_skin = GetShaderFilepath("VS_Skinning.cso");

	UINT shader_position_color = renderer.CreateShaderProgram(
		vs.c_str(), 
		ps.c_str(), 
		systems::SceneObjectRenderSystem::GetPerInstanceSize());

	//UINT shader_skinning = renderer.CreateShaderProgram(
	//	vs_skin.c_str(),
	//	ps.c_str(),
	//	systems::UnitRenderSystem::GetPerInstanceSize());



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

	const UINT gpu_memory_max = 1000000;
	UINT gpu_memory_usage = 0;
	char* p_gpu_memory = (char*)malloc(gpu_memory_max);

	UINT offset = 0;
	graphics::MeshRegion mesh_regions[] = { mesh_region_hexagon, mesh_region_tree, mesh_region_rock };

	// World tiles and scenery
	UINT instance_count_scenery[] = { ecs.getComponentCountOfType(components::TileComponent::typeID), 6, 6 };
	graphics::ShaderModelLayout layout_scenery = { 0 };
	layout_scenery.MeshCount = 3;
	layout_scenery.pMeshes = mesh_regions;
	layout_scenery.pInstanceCountPerMesh = instance_count_scenery;

	systems::TileRenderSystem* p_tile_renderer = ecs.createSystem<systems::TileRenderSystem>(9);
	systems::SceneObjectRenderSystem* p_scenery_renderer = ecs.createSystem<systems::SceneObjectRenderSystem>(9);

	// Units
	//UINT instance_count_buddies = 12;
	//graphics::ShaderModelLayout layout_skin = { 0 };
	//layout_skin.MeshCount = 1;
	//layout_skin.pMeshes = &mesh_region_dude;
	//layout_skin.pInstanceCountPerMesh = &instance_count_buddies;


	systems::UnitRenderSystem* p_unit_renderer = ecs.createSystem<systems::UnitRenderSystem>(9);
	p_unit_renderer->Initialize(&renderer);


	//// Map tiles will be uploaded with ocean meshes
	//UINT index = 0;
	//for (FilteredEntity tile: tile_iterator.entities)
	//{
	//	components::TileComponent* p_tile_comp = tile.getComponent<components::TileComponent>();
	//	components::TransformComponent* p_transform_comp = tile.getComponent<components::TransformComponent>();
	//	components::ColorComponent* p_color_comp = tile.getComponent<components::ColorComponent>();

	//	pData[index].x = p_transform_comp->position.x;
	//	pData[index].y = p_transform_comp->position.y;
	//	pData[index].z = p_transform_comp->position.z;

	//	int random = rand() % 101;
	//	int color_offset = -50 + random;
	//	switch (p_tile_comp->tileType)
	//	{
	//	case TileTypes::GAME_FIELD:
	//		pData[index].Color = PACK(p_color_comp->red, p_color_comp->green, p_color_comp->blue, 0);
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

	//TypeFilter scene_object_filter;
	//scene_object_filter.addRequirement(components::SceneObjectComponent::typeID);
	//scene_object_filter.addRequirement(components::TransformComponent::typeID);
	//scene_object_filter.addRequirement(components::ColorComponent::typeID);
	//EntityIterator scene_object_iterator = ecs.getEntititesByFilter(scene_object_filter);

	//UINT scene_objects_index = tile_count;
	//for (FilteredEntity object : scene_object_iterator.entities)
	//{
	//	components::SceneObjectComponent* p_obj_comp = object.getComponent<components::SceneObjectComponent>();
	//	components::TransformComponent* p_transform_comp = object.getComponent<components::TransformComponent>();
	//	components::ColorComponent* p_color_comp = object.getComponent<components::ColorComponent>();

	//	pData[scene_objects_index].x = p_transform_comp->position.x;
	//	pData[scene_objects_index].y = p_transform_comp->position.y;
	//	pData[scene_objects_index].z = p_transform_comp->position.z;

	//	pData[scene_objects_index].Color = PACK(p_color_comp->red, p_color_comp->green, p_color_comp->blue, 0);
	//	scene_objects_index++;
	//}


	//sizeOfPdata += (256 - (sizeOfPdata % 256));

	//SkinningShaderProgramInput* skin_shader_program_input = new SkinningShaderProgramInput[12];
	//UINT skin_size = sizeof(SkinningShaderProgramInput) * 12;
	//ModelLoader::Skeleton* skeleton = MeshContainer::GetMesh(MESH_TYPE_UNIT)->GetSkeleton();

	//InitAnimation(skeleton, skin_shader_program_input);


	renderer.SetShaderModelLayout(shader_position_color, layout_scenery);
	//renderer.SetShaderModelLayout(shader_skinning, layout_skin);

	//char* pInstanceData = (char*)malloc(skin_size + sizeOfPdata);
	//memcpy(pInstanceData, pData, sizeOfPdata);

	//memcpy(pInstanceData + sizeOfPdata, skin_shader_program_input, skin_size);
	
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
			


			//TypeFilter army_filter;
			//army_filter.addRequirement(components::UnitComponent::typeID);
			//army_filter.addRequirement(components::TransformComponent::typeID);
			//EntityIterator unit_iterator = ecs.getEntititesByFilter(army_filter);

			//int unitIndex = 0;
			//for (FilteredEntity unit : unit_iterator.entities)
			//{
			//	components::TransformComponent* p_transform_comp = unit.getComponent<ecs::components::TransformComponent>();

			//	XMMATRIX world = XMMatrixIdentity();
			//	world *= XMMatrixScaling(p_transform_comp->scale.x, p_transform_comp->scale.y, p_transform_comp->scale.z);
			//	world *= XMMatrixRotationRollPitchYaw(p_transform_comp->rotation.x, p_transform_comp->rotation.y, p_transform_comp->rotation.z);
			//	world *= XMMatrixTranslation(p_transform_comp->position.x, p_transform_comp->position.y, p_transform_comp->position.z);

			//	XMStoreFloat4x4(&skin_shader_program_input[unitIndex++].world, world);
			//}

			//UpdateAnimation(skeleton, skin_shader_program_input, frame_count2);

			//memcpy(pInstanceData + sizeOfPdata, skin_shader_program_input, skin_size);

			gpu_memory_usage = 0;

			p_tile_renderer->SetBegin(p_gpu_memory);
			gpu_memory_usage += ecs.getComponentCountOfType(components::TileComponent::typeID) * sizeof(float) * 4;
			p_scenery_renderer->SetBegin(p_gpu_memory + gpu_memory_usage);
			gpu_memory_usage += ecs.getComponentCountOfType(components::SceneObjectComponent::typeID) * sizeof(float) * 4;

			gpu_memory_usage = PAD(gpu_memory_usage, 256);

			p_unit_renderer->SetBegin(p_gpu_memory + gpu_memory_usage);
			gpu_memory_usage += ecs.getComponentCountOfType(components::UnitComponent::typeID) * sizeof(SkinningShaderProgramInput);

			ecs.update(0.002f);


			renderer.BeginUpload();
			//renderer.UploadPerInstanceData(pInstanceData, sizeOfPdata + skin_size, 0);
			renderer.UploadPerInstanceData(p_gpu_memory, gpu_memory_usage, 0);

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