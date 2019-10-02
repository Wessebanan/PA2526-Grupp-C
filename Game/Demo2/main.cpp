#include "ecs.h"
#include "rendering/RenderManager.h"
#include "Mesh.h"
#include "GridFunctions.h"
#include "CameraFunctions.h"
#include "InitInputHandler.h"


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
			{ components::TransformComponent::typeID, components::TransformComponent::size, 1 + (gridHeight * gridWidth)},
		};
		ecsDesc.compTypeCount = 2;
		ecsDesc.compTypeMemDescs = ecsMemDesc;
		ecsDesc.systemLayerCount = 10;
		ecs.initialize(ecsDesc);
	}
	GridFunctions::CreateGrid(ecs, gridWidth, gridHeight, 4);


	initInputECS(ecs, new InputBackend());


	//{
	//	//Initialize components
	//	TransformComponent transform;
	//	transform.position = CameraDefines::originalPosition;
	//	transform.rotation = CameraDefines::originalRotation;
	//	transform.scale = CameraDefines::originalScale;
	//	CameraComponent camera;
	//	camera.target = CameraDefines::originalTarget;
	//	camera.up = CameraDefines::originalUp;
	//	camera.forward = CameraDefines::originalForward;
	//	camera.right = CameraDefines::originalRight;
	//	XMVECTOR cam_pos = XMVectorSet(transform.position.x, transform.position.y, transform.position.z, 0.0f);
	//	XMVECTOR target = XMLoadFloat4(&camera.target);
	//	XMVECTOR up = XMLoadFloat4(&camera.up);
	//	XMMATRIX view = XMLoadFloat4x4(&camera.viewMatrix);
	//	XMMATRIX projection = XMLoadFloat4x4(&camera.projectionMatrix);

	//	//Set the view and projection matrix in the CameraComponent.
	//	view = XMMatrixLookAtLH(cam_pos, target, up);
	//	projection = XMMatrixPerspectiveFovLH(CameraDefines::fovAngle, CameraDefines::aspectRatio, CameraDefines::nearPlane, CameraDefines::farPlane);

	//	//Store the values in the component.
	//	XMStoreFloat4(&camera.target, target);
	//	XMStoreFloat4(&camera.up, up);
	//	XMStoreFloat4x4(&camera.viewMatrix, view);
	//	XMStoreFloat4x4(&camera.projectionMatrix, projection);

	//	//Create the camera entity
	//	ecs.createEntity(transform, camera);
	//}
	ecs.createSystem<ecs::systems::UpdateCameraSystem>();
	ecs.update(0.01f);



	using namespace rendering;
	using namespace DirectX;

	RenderManager mng;
	mng.Initialize(1280, 720, "D3D11");

	ModelLoader::Mesh tile("../hexagon_tile.fbx");
	//ModelLoader::Mesh tile("../hexTile3.fbx");

	int mesh_tile; // tile
	{
		VERTEX_BUFFER_DATA vertex_data = { NULL };
		vertex_data.VertexCount = tile.GetVertexPositionVector()->size() * 12;
		vertex_data.pVertexData = tile.GetVertexPositionVector()->data();

		INDEX_BUFFER_DATA index_data = { NULL };
		index_data.IndexCount = tile.GetIndexVector()->size() * 12;
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



	CameraFunctions::CreateDevCamera(ecs);


	float x = 2.0f;
	float z = 1.0f;
	//XMFLOAT4X4 viewMatrix;
	//XMStoreFloat4x4(&viewMatrix,
	//	XMMatrixLookToLH(
	//		{ x, 2.0f, z },
	//		{ 0.0f, 0.0f,  1.0f },
	//		{ 0.0f, 1.0f,  0.0f }
	//));
	//mng.SetViewMatrix(viewMatrix);
	ecs::ComponentIterator itt2 = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	components::CameraComponent* camComp;
	camComp = (components::CameraComponent*)itt2.next();
	ecs.update(0.1f);
	mng.SetViewMatrix(camComp->viewMatrix);


	pWnd->Show();
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			mng.Clear();

			float moveSpeed = 0.01f;
			//if (GetAsyncKeyState(VK_UP))
			//{
			//	z += moveSpeed;
			//}
			//if (GetAsyncKeyState(VK_DOWN))
			//{
			//	z -= moveSpeed;
			//}

			//if (GetAsyncKeyState(VK_LEFT))
			//{
			//	x -= moveSpeed;
			//}
			//if (GetAsyncKeyState(VK_RIGHT))
			//{
			//	x += moveSpeed;
			//}
			//ecs::ComponentIterator ittKey = ecs.getAllComponentsOfType(ecs::components::KeyboardComponent::typeID);
			//components::KeyboardComponent* kbComp;
			//if (kbComp = (components::KeyboardComponent*)ittKey.next())//(inp->mpExitKey->key.pressed)//(kbComp->ECS)
			//{
			//	if (kbComp->W)
			//	{
			//		z += moveSpeed;
			//	}
			//	if (kbComp->S)
			//	{
			//		z -= moveSpeed;
			//	}

			//	if (kbComp->A)
			//	{
			//		x -= moveSpeed;
			//	}
			//	if (kbComp->D)
			//	{
			//		x += moveSpeed;
			//	}
			//}
			//XMStoreFloat4x4(&viewMatrix,
			//	XMMatrixLookToLH(
			//		{ x, 0.0f, z },
			//		{ 0.0f, 0.0f,  1.0f },
			//		{ 0.0f, 1.0f,  0.0f }
			//));
			//itt = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
			//components::CameraComponent* camComp;
			//camComp = (components::CameraComponent*)itt.next();
			mng.SetViewMatrix(camComp->viewMatrix);
			//mng.SetViewMatrix(viewMatrix);

			mng.Draw();
			pWnd->Present();


			ecs.update(0.1f);



		}
	}

	mng.Destroy();

	return 0;

}