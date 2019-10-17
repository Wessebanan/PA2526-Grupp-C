
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


graphics::MeshRegion InsertMesh(ModelLoader::Mesh& mesh, graphics::MeshManager& rMng)
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
	mesh_manager.Initialize(1000, 1000);

	UINT pipeline_shadow_map;
	{
		graphics::SHADOW_MAP_PIPELINE_DESC desc;
		desc.PixelsWidth	= 2048;
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

	const std::string vs_skin = GetShaderFilepath("VS_Skinning.cso");
	UINT shader_index1 = renderer.CreateShaderProgram(
		vs_skin.c_str(),
		ps.c_str(),
		sizeof(float) * 3 + sizeof(UINT));

	ModelLoader::Mesh mesh_hexagon("../meshes/hexagon.fbx");
	ModelLoader::Mesh mesh_rock("../meshes/rock.fbx");
	ModelLoader::Mesh mesh_tree("../meshes/tree.fbx");
	ModelLoader::Mesh mesh_dude("../Walking2.fbx");
	graphics::MeshRegion mesh_region_hexagon	= InsertMesh(mesh_hexagon, mesh_manager);
	graphics::MeshRegion mesh_region_tree		= InsertMesh(mesh_rock, mesh_manager);
	graphics::MeshRegion mesh_region_rock		= InsertMesh(mesh_tree, mesh_manager);
	graphics::MeshRegion mesh_region_dude	    = InsertMesh(mesh_dude, mesh_manager);

	ecs::EntityComponentSystem ecs;

	//Tiles + sceneobjects + units + camera
	ecs.reserveComponentCount<ecs::components::TransformComponent>(144 + 12 + 12 + 1);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(144 + 12 + 12);
	ecs.reserveComponentCount<ecs::components::TileComponent>(144);

	InitSound(ecs);

	InitInput(ecs);
	InitInterpreter(ecs);

	InitGrid(ecs);
	InitArmy(ecs);
	InitSceneObjects(ecs);

	InitCamera(ecs);

	//ModelLoader::Mesh **pp_meshes = InitMesh(ecs, mesh_manager);
	//
	//InitPhysics(ecs, pp_meshes);

	//PlaceMesh(ecs, pMng);


	// to get components in the loop
	ecs::ComponentIterator itt;

	itt = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	ecs::components::CameraComponent* p_cam_comp = (ecs::components::CameraComponent*)itt.next();


	graphics::ShaderModelLayout layout;
	layout.MeshCount			= 3;
	layout.Meshes[0]			= (mesh_region_hexagon);
	layout.Meshes[1]			= (mesh_region_tree);
	layout.Meshes[2]			= (mesh_region_rock);

	graphics::ShaderModelLayout layout_skin;
	layout.MeshCount			= 1;
	layout.Meshes[0]			= (mesh_region_dude);



	struct ShaderProgramInput
	{
		float x, y, z;
		uint32_t Color;
	};

	struct SkinningShaderProgramInput
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 boneMatrices[63];
	};

	UINT index = 0;
	itt = ecs.getAllComponentsOfType(ecs::components::TileComponent::typeID);
	ecs::components::TileComponent* tileComp;
	size_t size = ecs.getComponentCountOfType(ecs::components::TileComponent::typeID);

	ShaderProgramInput* pData = new ShaderProgramInput[size];
	layout.InstanceCounts[0] = size;
	layout.InstanceCounts[1] = 0;
	layout.InstanceCounts[2] = 0;


	layout.TotalModels = size;

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

	SkinningShaderProgramInput skin_shader_program_input[12];
	ModelLoader::Skeleton* skeleton = mesh_dude.GetSkeleton();
	// Load animation data for first frame
	for (unsigned int i = 0; i < 12; ++i)
	{
		memcpy(&skin_shader_program_input[i].boneMatrices[0], skeleton->animationData, skeleton->jointCount * sizeof(XMFLOAT4X4));
		
	}

	renderer.SetShaderModelLayout(shader_index0, layout);
	renderer.SetShaderModelLayout(shader_index1, layout_skin);
	renderer.SetModelData(pData, sizeof(ShaderProgramInput) * size);

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