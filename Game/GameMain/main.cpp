//#include "Input/InitInput.h"
#include "../../Graphics/includes/Window.h"
//#include "ecs.h"



//#include "gameRendering/InitMesh.h"
//#include "gameRendering/PlaceMesh.h"

//#include "gameAI/InitArmy.h"
//#include "gameAI/InitGrid.h"

//#include "Input/InitInterpreter.h"

//#include "gameUtility/InitCamera.h"

//#include "gameSceneObjects/InitSceneObjectsh.h"

//#include "gameAudio/InitAudio.h"

//#include "Physics/InitPhysics.h"

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

int main()
{
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
		desc.Height			= 25.0f;
		desc.NearPlane		= 0.1f;
		desc.FarPlane		= 100.0f;
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

	UINT shader_index0 = renderer.CreateShaderProgram(
		"VS_Default.cso",
		"PS_Default.cso",
		sizeof(float) * 3 + sizeof(UINT));

	graphics::MeshRegion mesh_region0;
	{
		ModelLoader::Mesh mesh0("hexagon.fbx");

		mesh_region0 = mesh_manager.CreateMeshRegion(
			mesh0.GetVertexPositionVector()->size(),
			mesh0.GetIndexVector()->size());

		{
			graphics::VERTEX_DATA data = { NULL };
			data.pVertexPositions	= mesh0.GetVertexPositionVector()->data();
			data.pVertexNormals		= mesh0.GetNormalVector()->data();
			data.pVertexTexCoords	= mesh0.GetUVVector()->data();

			mesh_manager.UploadData(
				mesh_region0,
				data,
				mesh0.GetIndexVector()->data());
		}
	}

	graphics::ShaderModelLayout layout;
	layout.Meshes[0] = (mesh_region0);

	layout.InstanceCounts[0] = (2);

	layout.MeshCount	= 1;
	layout.TotalModels	= 2;


	struct ShaderProgramInput
	{
		float x, y, z;
		uint32_t Color;
	};

	ShaderProgramInput v[2];
	v[0].x = 0.0f;
	v[0].y = 0.0f;
	v[0].z = 2.0f;
	v[0].Color = PACK(255, 0, 0, 255);

	v[1].x = 0.0f;
	v[1].y = 0.0f;
	v[1].z = 5.0f;
	v[1].Color = PACK(0, 255, 0, 255);

	renderer.SetShaderModelLayout(shader_index0, layout);
	renderer.SetModelData(v, sizeof(v));

	//srand(time(0));

	//EntityComponentSystem ecs;

	// Tiles + sceneobjects + units + camera
	//ecs.reserveComponentCount<ecs::components::TransformComponent>(144 + 12 + 12 + 1);
	//ecs.reserveComponentCount<ecs::components::ColorComponent>(144 + 12 + 12);
	//ecs.reserveComponentCount<ecs::components::TileComponent>(144);

	//InitSound(ecs);

	//InitInput(ecs);
	//InitInterpreter(ecs);
	//CameraFunctions::CreateDevCamera(ecs);


	//InitGrid(ecs);
	//InitArmy(ecs);
	//InitSceneObjects(ecs);


	//rendering::SUN_DESC sun_desc;
	//sun_desc.Red = 200;
	//sun_desc.Green = 200;
	//sun_desc.Blue = 200;
	//
	//sun_desc.Direction	= { 0.8f, -1.0f, 0.0f };
	//sun_desc.Position	= { -4.0f, 8.0f, 10.0f };

	//sun_desc.NearPlane = 0.1f;
	//sun_desc.FarPlane = 100.0f;

	//sun_desc.Width = 25.0f;
	//sun_desc.Height = 25.0f;

	//sun_desc.Resolution = 2048;
	//
	//rendering::RenderManager* pMng = new rendering::RenderManager;
	//pMng->Initialize(sun_desc, 1600, 900, "D3D11");

	//

	//InitCamera(ecs);

	//ModelLoader::Mesh **pp_meshes = InitMesh(ecs, pMng);
	//
	//InitPhysics(ecs, pp_meshes);

	//graphics::PresentWindow* pWnd = pMng->GetPresentWindow();

	//PlaceMesh(ecs, pMng);


	//// to get components in the loop
	//ecs::ComponentIterator itt;

	//itt = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	//ecs::components::CameraComponent* p_cam_comp = (ecs::components::CameraComponent*)itt.next();

	float x = -5.0f, y = 10.0f, z = 0.0f;
	DirectX::XMFLOAT4X4 camera_matrix;
	SetViewMatrix(
		camera_matrix, 
		x, y, z,
		1.0f, -1.0f, 0.0f);

	//pMng->SetViewMatrix(p_cam_comp->viewMatrix);
	wnd.Open();
	//ecs.update(0.1f);
	while (wnd.IsOpen())
	{
		if (!wnd.Update()) 
		{
			//ecs.update(0.1f);

			if (GetAsyncKeyState(VK_ESCAPE))
			{
				wnd.Close();
			}

			{	
				DirectX::XMFLOAT4X4 shadow_matrix;
				SetViewMatrix(
					shadow_matrix,
					0.0f, 4.0f, -5.0f,
					0.0f, -1.0f, 1.0f);

				graphics::SHADOW_MAP_PIPELINE_DATA data;
				data.ViewMatrix = shadow_matrix;

				renderer.UpdatePipeline(pipeline_shadow_map, &data);
			}

			{
				graphics::FORWARD_RENDERING_PIPELINE_DATA data;
				data.ViewMatrix = camera_matrix;
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