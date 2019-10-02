#include "ecs.h"
#include "rendering/RenderManager.h"
#include "Mesh.h"
#include "GridFunctions.h"
//#include "CameraFunctions.h"



int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	UINT gridWidth = 12;
	UINT gridHeight = 12;
	
	ecs::EntityComponentSystem ecs;
	{
		ecs::ECSDesc ecsDesc;
		ecs::CompTypeMemDesc ecsMemDesc[] =
		{
			{ components::WorldComponent::typeID, components::WorldComponent::size, systems::compCount},
			{ components::MeshComponent::typeID, components::MeshComponent::size, systems::compCount},
		};
		ecsDesc.compTypeCount = 2;
		ecsDesc.compTypeMemDescs = ecsMemDesc;
		ecsDesc.systemLayerCount = 10;
		myECS.initialize(ecsDesc);
	}
	//CameraFunctions::CreateDevCamera(ecs);

	GridFunctions::CreateGrid(ecs, gridWidth, gridHeight, 8);






	using namespace rendering;
	using namespace DirectX;

	RenderManager mng;
	mng.Initialize(1280, 720, "D3D11");

	ModelLoader::Mesh tile("../hexTile3.fbx");

	int mesh_tile; // tile
	{
		VERTEX_BUFFER_DATA vertex_data = { NULL };
		vertex_data.VertexCount = tile.GetVertexPositionVector()->size();
		vertex_data.pVertexData = tile.GetVertexPositionVector()->data();

		INDEX_BUFFER_DATA index_data = { NULL };
		index_data.IndexCount = tile.GetIndexVector()->size();
		index_data.pIndexData = tile.GetIndexVector()->data();

		mesh_tile = mng.CreateMesh(
			&vertex_data, 
			&index_data);
	}

	struct float4
	{
		float x, y, z, w;
	};

	TECHNIQUE_HEAP_LAYOUT_DESC desc[RENDER_TECHNIQUES_COUNT] = { 0 };

	// Default Technique will render 'count' meshes in white
	MODEL_LAYOUT_DESC m_desc;
	m_desc.InstanceCount = gridWidth * gridHeight;
	m_desc.MeshIndex = mesh_tile;

	desc[RENDER_DEFAULT].PerInstanceByteWidth = sizeof(float4);
	desc[RENDER_DEFAULT].pModelLayout = &m_desc;
	desc[RENDER_DEFAULT].ModelLayoutCount = 1;

	mng.CreateModelHeap(desc);

	graphics::PresentWindow* pWnd = mng.GetPresentWindow();

	float4* pTilePosition = (float4*)mng.GetTechniqueModelBuffer(RENDER_DEFAULT);

	UINT index = 0;
	ecs::ComponentIterator itt = ecs.getAllComponentsOfType(ecs::components::TransformComponent::typeID);
	ecs::components::TransformComponent* trComp;
	while (trComp = (ecs::components::TransformComponent*)itt.next())
	{
		pTilePosition[index].x = trComp->position.x;
		pTilePosition[index].y = trComp->position.y;
		pTilePosition[index].z = trComp->position.z;
		pTilePosition[index].w = 1.0f;

		index++;
	}


	pTilePosition->z = 10.0f;
	pTilePosition->w = 1.0f;

	float x = 2.0f;
	float z = 1.0f;
	XMFLOAT4X4 viewMatrix;
	XMStoreFloat4x4(&viewMatrix,
		XMMatrixLookToLH(
			{ x, 2.0f, z },
			{ 0.0f, 0.0f,  1.0f },
			{ 0.0f, 1.0f,  0.0f }
	));

	mng.SetViewMatrix(viewMatrix);

	pWnd->Show();
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			mng.Clear();

			float moveSpeed = 0.01f;
			if (GetAsyncKeyState(VK_UP))
			{
				z += moveSpeed;
			}
			if (GetAsyncKeyState(VK_DOWN))
			{
				z -= moveSpeed;
			}

			if (GetAsyncKeyState(VK_LEFT))
			{
				x -= moveSpeed;
			}
			if (GetAsyncKeyState(VK_RIGHT))
			{
				x += moveSpeed;
			}

			XMStoreFloat4x4(&viewMatrix,
				XMMatrixLookToLH(
					{ x, 0.0f, z },
					{ 0.0f, 0.0f,  1.0f },
					{ 0.0f, 1.0f,  0.0f }
			));

			mng.SetViewMatrix(viewMatrix);

			mng.Draw();
			pWnd->Present();
		}
	}

	mng.Destroy();

	return 0;
}