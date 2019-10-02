#include "ecs.h"
#include "rendering/RenderManager.h"
#include "Mesh.h"
#include "GridFunctions.h"
#include "CameraFunctions.h"
#include "InitInputHandler.h"
#include "AIFunctions.h"


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	UINT gridWidth = 12;
	UINT gridHeight = 12;

	using namespace ecs;
	ecs::EntityComponentSystem ecs;
	{
		ecs::ECSDesc ecsDesc;
		ecs::CompTypeMemDesc ecsMemDesc[] =
		{
			{ components::TileComponent::typeID, components::TileComponent::size, (gridHeight * gridWidth)},
			{ components::TransformComponent::typeID, components::TransformComponent::size, 1 + (gridHeight * gridWidth) + 12},
		};
		ecsDesc.compTypeCount = 2;
		ecsDesc.compTypeMemDescs = ecsMemDesc;
		ecsDesc.systemLayerCount = 10;
		ecs.initialize(ecsDesc);
	}
	GridFunctions::CreateGrid(ecs, gridWidth, gridHeight, 1.1f);


	initInputECS(ecs, new InputBackend());


	CameraFunctions::CreateDevCamera(ecs);

	ecs.createSystem<ecs::systems::UpdateCameraSystem>();
	//ecs.update(0.01f);


	AIFunctions::CreatePlayerArmies(ecs);




	ecs.update(0.1f);

	using namespace rendering;
	using namespace DirectX;

	RenderManager mng;
	mng.Initialize(1280, 720, "D3D11");


	ModelLoader::Mesh tile("../hexagon_tile2.fbx");



	ModelLoader::Mesh dude("../hexagon_tile.fbx");

	int mesh_dude; // dude
	{
		//struct float3
		//{
		//	float x, y, z;
		//};

		//float3 triangle[3] = {
		//	-0.1f,  -0.1f, 0.0f,
		//	 0.0f,   0.1f, 0.0f,
		//	 0.1f,  -0.1f, 0.0f,
		//};

		//int indexBuffer[3] = {
		//	0, 1, 2
		//};

		//VERTEX_BUFFER_DATA vertex_data = { NULL };
		//vertex_data.VertexCount = 3;
		//vertex_data.pVertexData = triangle;

		//INDEX_BUFFER_DATA index_data = { NULL };
		//index_data.IndexCount = 3 * sizeof(int);
		//index_data.pIndexData = indexBuffer;

		//mesh_dude = mng.CreateMesh(
		//	&vertex_data,
		//	//NULL);
		//	&index_data);

		VERTEX_BUFFER_DATA vertex_data = { NULL };
		vertex_data.VertexCount = dude.GetVertexPositionVector()->size() * 4;
		vertex_data.pVertexData = dude.GetVertexPositionVector()->data();

		INDEX_BUFFER_DATA index_data = { NULL };
		index_data.IndexCount = dude.GetIndexVector()->size() * 4;
		index_data.pIndexData = dude.GetIndexVector()->data();

		mesh_dude = mng.CreateMesh(
			&vertex_data,
			&index_data);
	}

	int mesh_tile; // tile
	{
		VERTEX_BUFFER_DATA vertex_data = { NULL };
		vertex_data.VertexCount = tile.GetVertexPositionVector()->size() * 4;
		vertex_data.pVertexData = tile.GetVertexPositionVector()->data();

		INDEX_BUFFER_DATA index_data = { NULL };
		index_data.IndexCount = tile.GetIndexVector()->size() * 4;
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
	MODEL_LAYOUT_DESC m_desc[2];
	m_desc[0].InstanceCount = gridWidth * gridHeight;
	m_desc[0].MeshIndex = mesh_tile;

	m_desc[1].InstanceCount = 12;
	m_desc[1].MeshIndex = mesh_dude;

	desc[RENDER_DEFAULT].PerInstanceByteWidth = sizeof(float4);
	desc[RENDER_DEFAULT].pModelLayout = m_desc;
	desc[RENDER_DEFAULT].ModelLayoutCount = 2;

	mng.CreateModelHeap(desc);

	graphics::PresentWindow* pWnd = mng.GetPresentWindow();

	float4* pTilePosition = (float4*)mng.GetTechniqueModelBuffer(RENDER_DEFAULT);

	UINT index = 0;
	ecs::ComponentIterator itt;
	
	itt = ecs.getAllComponentsOfType(ecs::components::TileComponent::typeID);
	ecs::components::TileComponent* tileComp;
	while (tileComp = (ecs::components::TileComponent*)itt.next())
	{
		ecs::components::TransformComponent* trComp = ecs.getComponentFromEntity<ecs::components::TransformComponent>(tileComp->getEntityID());

		pTilePosition[index].x = trComp->position.x;
		pTilePosition[index].y = trComp->position.y;
		pTilePosition[index].z = trComp->position.z;
		pTilePosition[index].w = index / (float)(gridHeight*gridWidth);

		index++;
	}

	int armyIndex = 0;
	itt = ecs.getAllComponentsOfType(ecs::components::ArmyComponent::typeID);
	ecs::components::ArmyComponent* armComp;
	while (armComp = (ecs::components::ArmyComponent*)itt.next())
	{
		for (size_t i = 0; i < 3; i++)
		{
			ecs::components::TransformComponent* trComp = ecs.getComponentFromEntity<ecs::components::TransformComponent>(armComp->unitIDs[i]);

			pTilePosition[index].x = trComp->position.x;
			pTilePosition[index].y = trComp->position.y;
			pTilePosition[index].z = trComp->position.z;
			pTilePosition[index].w = (1.0f / (armyIndex + 0.1f)) + 0.1f;

			index++;
		}
		armyIndex++;
	}

	//for (size_t i = 0; i < 4; i++)
	//{
	//	for (size_t j = 0; j < 3; j++)
	//	{
	//		pTilePosition[index].x = (i % 2) * gridWidth + j;
	//		pTilePosition[index].y = 10;
	//		pTilePosition[index].z = ((i % 2)) * gridHeight + j;
	//		pTilePosition[index].w = 1.0f / i;

	//		index++;
	//	}

	//}


	itt = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	components::CameraComponent* camComp;
	camComp = (components::CameraComponent*)itt.next();
	mng.SetViewMatrix(camComp->viewMatrix);


	pWnd->Show();
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			mng.Clear();

			float moveSpeed = 0.01f;

			mng.SetViewMatrix(camComp->viewMatrix);

			mng.Draw();
			pWnd->Present();

			ecs.update(0.1f);
		}
	}

	mng.Destroy();

	return 0;

}