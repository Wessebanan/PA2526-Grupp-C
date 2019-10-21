
#include "Input/InitInput.h"
#include "../../Graphics/includes/Window.h"
#include "ecs.h"



//#include "gameRendering/InitMesh.h"
//#include "gameRendering/PlaceMesh.h"

#include "gameAI/InitArmy.h"
#include "gameAI/InitGrid.h"
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
	ModelLoader::Mesh mesh_hexagon("../meshes/hexagon_tile5.fbx");
	ModelLoader::Mesh mesh_rock("../meshes/rock.fbx");
	ModelLoader::Mesh mesh_tree("../meshes/tree2.fbx");
	ModelLoader::Mesh mesh_dude("../RunningCustom2.fbx");


	graphics::MeshRegion mesh_region_hexagon	= UploadMeshToGPU(mesh_hexagon, mesh_manager);
	graphics::MeshRegion mesh_region_tree		= UploadMeshToGPU(mesh_tree, mesh_manager);
	graphics::MeshRegion mesh_region_rock		= UploadMeshToGPU(mesh_rock, mesh_manager);
	graphics::MeshRegion mesh_region_dude		= UploadMeshToGPU(mesh_dude, mesh_manager);

	// Will be moved to a system
	UINT pipeline_shadow_map;
	{
		graphics::SHADOW_MAP_PIPELINE_DESC desc;
		desc.PixelsWidth	= 4096;
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

	struct ShaderProgramInput
	{
		float x, y, z;
		uint32_t Color;
	};



	const std::string vs = GetShaderFilepath("VS_Default.cso");
	const std::string ps = GetShaderFilepath("PS_Default.cso");
	const std::string vs_skin = GetShaderFilepath("VS_Skinning.cso");

	UINT shader_position_color = renderer.CreateShaderProgram(
		vs.c_str(), 
		ps.c_str(), 
		sizeof(ShaderProgramInput));

	UINT shader_skinning = renderer.CreateShaderProgram(
		vs_skin.c_str(),
		ps.c_str(),
		sizeof(SkinningShaderProgramInput));



	/* ECS BEGIN */

	ecs::EntityComponentSystem ecs;

	//Tiles + sceneobjects + units + camera
	ecs.reserveComponentCount<ecs::components::TransformComponent>(144 + 12 + 12 + 1);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(144 + 12 + 12);
	ecs.reserveComponentCount<ecs::components::TileComponent>(144);

	InitSound(ecs);

	ecs.createSystem<ecs::systems::PathfindingStateSystem>(5);
	ecs.createSystem<ecs::systems::IdleStateSystem>(5);
	ecs.createSystem<ecs::systems::MoveStateSystem>(5);
	ecs.createSystem<ecs::systems::AttackStateSystem>(5);
	ecs.createSystem<ecs::systems::SwitchStateSystem>(4);
	//ecs.createSystem<ecs::systems::DynamicMovementSystem>();
	//ecs.createSystem<ecs::systems::GroundCollisionSystem>();

	InitInput(ecs);
	InitInterpreter(ecs);

	InitGrid(ecs);
	InitArmy(ecs);
	InitSceneObjects(ecs);

	InitCamera(ecs);

	InitPhysics(ecs, &mesh_dude);

	// to get components in the loop
	ecs::ComponentIterator itt;

	itt = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	ecs::components::CameraComponent* p_cam_comp = (ecs::components::CameraComponent*)itt.next();

	itt = ecs.getAllComponentsOfType(ecs::components::TileComponent::typeID);
	ecs::components::TileComponent* tileComp;

	/* ECS END */

	size_t tile_count = ecs.getComponentCountOfType(ecs::components::TileComponent::typeID);

	UINT instance_count_scenery[] = { tile_count, 6, 6 };
	graphics::MeshRegion mesh_regions[] = { mesh_region_hexagon, mesh_region_tree, mesh_region_rock };
	graphics::ShaderModelLayout layout_scenery = { 0 };
	layout_scenery.MeshCount				= 3;
	layout_scenery.pMeshes					= mesh_regions;
	layout_scenery.pInstanceCountPerMesh	= instance_count_scenery;

	UINT instance_count_buddies = 12;
	graphics::ShaderModelLayout layout_skin = { 0 };
	layout_skin.MeshCount					= 1;
	layout_skin.pMeshes						= &mesh_region_dude;
	layout_skin.pInstanceCountPerMesh		= &instance_count_buddies;


	ShaderProgramInput* pData	= new ShaderProgramInput[tile_count + 6 + 6];
	UINT sizeOfPdata			= (UINT)(sizeof(ShaderProgramInput) * (tile_count + 6 + 6));
	sizeOfPdata = 2496;
	ZeroMemory(pData, sizeOfPdata);


	// Map tiles will be uploaded with ocean meshes
	UINT index = 0;
	while (tileComp = (ecs::components::TileComponent*)itt.next())
	{
		ecs::components::TransformComponent* trComp = ecs.getComponentFromEntity<ecs::components::TransformComponent>(tileComp->getEntityID());
		ecs::components::ColorComponent* color_comp = ecs.getComponentFromEntity<ecs::components::ColorComponent>(tileComp->getEntityID());

		pData[index].x = trComp->position.x;
		pData[index].y = trComp->position.y;
		pData[index].z = trComp->position.z;

		int random = rand() % 101;
		int color_offset = -50 + random;
		switch (tileComp->tileType)
		{
		case TileTypes::GAME_FIELD:
			pData[index].Color = PACK(color_comp->red, color_comp->green, color_comp->blue, 0);
			break;
		case TileTypes::WATER:
			pData[index].Color = PACK(0, 0, 200 + color_offset, 0);
			break;
		case TileTypes::UNDEFINED:
			pData[index].Color = PACK(0, 0, 0, 255);
			break;
		default:
			pData[index].Color = PACK(255, 255, 255, 255);
			break;
		}

		index++;
	}

	UINT scene_objects_index = tile_count;
	itt = ecs.getAllComponentsOfType(ecs::components::SceneObjectComponent::typeID);
	ecs::components::SceneObjectComponent* scene_comp;
	while (scene_comp = (ecs::components::SceneObjectComponent*)itt.next())
	{
		ecs::components::TransformComponent* trComp = ecs.getComponentFromEntity<ecs::components::TransformComponent>(scene_comp->getEntityID());
		ecs::components::ColorComponent* color_comp = ecs.getComponentFromEntity<ecs::components::ColorComponent>(scene_comp->getEntityID());

		pData[scene_objects_index].x = trComp->position.x;
		pData[scene_objects_index].y = trComp->position.y;
		pData[scene_objects_index].z = trComp->position.z;

		pData[scene_objects_index].Color = PACK(color_comp->red, color_comp->green, color_comp->blue, 0);


		scene_objects_index++;
	}


	sizeOfPdata += (256 - (sizeOfPdata % 256));

	SkinningShaderProgramInput* skin_shader_program_input = new SkinningShaderProgramInput[12];
	UINT skin_size = sizeof(SkinningShaderProgramInput) * 12;
	ModelLoader::Skeleton* skeleton = mesh_dude.GetSkeleton();

	InitAnimation(skeleton, skin_shader_program_input);


	renderer.SetShaderModelLayout(shader_position_color, layout_scenery);
	renderer.SetShaderModelLayout(shader_skinning, layout_skin);

	char* pInstanceData = (char*)malloc(skin_size + sizeOfPdata);
	memcpy(pInstanceData, pData, sizeOfPdata);

	memcpy(pInstanceData + sizeOfPdata, skin_shader_program_input, skin_size);
	
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
			ecs.update(0.002f);
			int armyIndex = 0;
			itt = ecs.getAllComponentsOfType(ecs::components::ArmyComponent::typeID);
			ecs::components::ArmyComponent* armComp;
			while (armComp = (ecs::components::ArmyComponent*)itt.next())
			{
				// over alla units in the army
				for (size_t i = 0; i < 3; i++)
				{
					ecs::components::TransformComponent* trComp = ecs.getComponentFromEntity<ecs::components::TransformComponent>(armComp->unitIDs[i]);

					XMMATRIX world = XMMatrixIdentity();
					world *= XMMatrixScaling(trComp->scale.x, trComp->scale.y, trComp->scale.z);
					world *= XMMatrixRotationRollPitchYaw(trComp->rotation.x, trComp->rotation.y, trComp->rotation.z);
					world *= XMMatrixTranslation(trComp->position.x, trComp->position.y, trComp->position.z);

					XMStoreFloat4x4(&skin_shader_program_input[armyIndex].world, world);

					armyIndex++;
				}
			}

			UpdateAnimation(skeleton, skin_shader_program_input, frame_count2);

			memcpy(pInstanceData + sizeOfPdata, skin_shader_program_input, skin_size);

			renderer.BeginUpload();
			renderer.UploadPerInstanceData(pInstanceData, sizeOfPdata + skin_size, 0);

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

			graphics::Present(0);

			frame_count++;
			if (frame_count % 3 == 0)
			{
				frame_count2++;
			}
		}
	}
	
	//for (int i = 0; i < Mesh::N_MESHES; i++) delete pp_meshes[i];
	//delete[] pp_meshes;

	mesh_manager.Destroy();
	renderer.Destroy();
	return 0;
}