#include "InitInputHandler.h"
#include "ecs.h"
#include "Mesh.h"
#include "DeviceInterface.h"
#include "UtilityFunctions.h"
#include "CameraFunctions.h"
#include "GridFunctions.h"
#include <DebugInfo.h>
#include "Shaders.h"
#include "ecsUser.h"

namespace DInfo
{
	namespace Graphics
	{
		DINFO_ADD_PARAM(FPS, int, 0);
	}
}

#pragma comment (lib, "GraphicsEngine_d")
using namespace graphics;
int main()
{
	ecs::CompTypeMemDesc ecsMemDesc[] = {
		{ TileComponent::typeID, TileComponent::size, 1024 },
		{ TransformComponent::typeID, TransformComponent::size, 1024 },
	};

	ecs::ECSDesc ecsDesc;
	ecsDesc.compTypeCount = 2;
	ecsDesc.compTypeMemDescs = ecsMemDesc;
	ecsDesc.systemLayerCount = 10;

	ecs::EntityComponentSystem ecs;
	ecs.initialize(ecsDesc);

	InputBackend inp;
	initInputECS(ecs, &inp);
	CameraFunctions::CreateDevCamera(ecs);
	ecs.createSystem<ecs::systems::Mackes>();
	//CameraFunctions::CreateCameraSystems(ecs);
	ecs::components::CameraComponent* p_camera = (ecs::components::CameraComponent*)ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID).next();

//	ecs::ComponentIterator iter = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
//	ecs::BaseComponent* baseComp = iter.next();
//	ecs::components::CameraComponent* camera = static_cast<ecs::components::CameraComponent*>(baseComp);
	size_t system_count = ecs.getTotalSystemCount();





	//camera.
	DeviceInterface* pDevice;
	PresentWindow* pWindow;
	GraphicsPipeline* pPipeline, *pPipeline2;
	CreateDeviceInterface(&pDevice);
	ModelLoader::Mesh hexMesh;
	BufferRegion hexMeshBuffReg, dudeMeshBuffReg;

	CreateDeviceInterface(&pDevice);
	RenderContext* pContext = pDevice->GetRenderContext();
	RenderTarget backBuffer;
	DepthBuffer depthBuffer;
	pDevice->CreateDepthBuffer(1280, 720, &depthBuffer);

	pDevice->CreatePresentWindow(1280, 720, "D3D11", &backBuffer, &pWindow);
	
	pDevice->CreatePipeline(gVertexShader, gPixelShader, &pPipeline);
	pDevice->CreatePipeline(gVertexShader2, gPixelShader2, &pPipeline2);
	
	pContext->SetViewport(0, 0, 1280, 720);
	pContext->SetGraphicsPipeline(pPipeline);

	HRESULT hr = hexMesh.LoadFBX("Models\\hexTile3.fbx");
	std::vector<DirectX::XMFLOAT3>* pVertices = hexMesh.GetVertexPositionVector();

	if (!hexMesh.HasUVs())
	{
		pDevice->CreateMeshRegion(pVertices->size(), pVertices->data(), hexMesh.GetNormalVector()->data(), NULL, &hexMeshBuffReg);
	}
	else
	{
		pDevice->CreateMeshRegion(pVertices->size(), pVertices->data(), hexMesh.GetNormalVector()->data(), hexMesh.GetUVVector()->data(), &hexMeshBuffReg);
	}

	ModelLoader::Mesh dudeMesh;
	hr = dudeMesh.LoadFBX("Models\\dudeTest2Triangulated.fbx");
	std::vector<DirectX::XMFLOAT3>* pVertices2 = dudeMesh.GetVertexPositionVector();
	if (!dudeMesh.HasUVs())
	{
		pDevice->CreateMeshRegion(pVertices2->size(), pVertices2->data(), dudeMesh.GetNormalVector()->data(), NULL, &dudeMeshBuffReg);
	}
	else
	{
		pDevice->CreateMeshRegion(pVertices2->size(), pVertices2->data(), dudeMesh.GetNormalVector()->data(), dudeMesh.GetUVVector()->data(), &dudeMeshBuffReg);
	}


	pContext->UploadMeshesToGPU();

	BufferRegion worldMatrixBufferRegion;
	pDevice->CreateDynamicBufferRegion(sizeof(DirectX::XMFLOAT4X4) * 1024, NULL, &worldMatrixBufferRegion);
	
	
	DirectX::XMFLOAT4X4 world(1.0f, 0.0f, 0.0f, 0.0f,
							  0.0f, 1.0f, 0.0f, 0.0f,
							  0.0f, 0.0f, 1.0f, 0.0f,
							  0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMStoreFloat4x4(&world,
		DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(2.0f, 2.0f, 2.0f), DirectX::XMMatrixRotationRollPitchYaw(-1.5708f, 0.0f, -1.5708f)));

	DirectX::XMFLOAT4X4 worldMatrices[1024];
	
	GridFunctions::CreateGrid(ecs, 32, 32, 2.0f);
	ecs::ComponentIterator comp_iter = ecs.getAllComponentsOfType(ecs::components::TileComponent::typeID);
	ecs::BaseComponent* p;
	ecs::components::TransformComponent* pTransform;
	DirectX::XMFLOAT3 tile_positions[1024];
	int counter = 0;
	while (p = comp_iter.next())
	{
		pTransform = static_cast<ecs::components::TransformComponent*>(ecs.getComponentFromEntity(ecs::components::TransformComponent::typeID, p->getEntityID()));
		tile_positions[counter] = pTransform->position;
		counter++;
	}
	
	for (int i = 0; i < 32; ++i)
	{
		for (int j = 0; j < 32; ++j)
		{
			if (i % 2 == 0) {

				DirectX::XMStoreFloat4x4(&worldMatrices[i* 32 + j], DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&world), DirectX::XMMatrixTranslation(j * 3.2f, 0.0f, i * 3.2f)));

			}
			else {
				DirectX::XMStoreFloat4x4(&worldMatrices[i * 32 + j], DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&world), DirectX::XMMatrixTranslation(j * 3.2f + 1.5f, 0.0f, i * 3.1f)));
			}
			DirectX::XMStoreFloat4x4(&worldMatrices[i * 32 + j], DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&world), DirectX::XMMatrixTranslation(tile_positions[i * 32 + j].x, tile_positions[i * 32 + j].y, tile_positions[i * 32 + j].z)));
		}
	}

	BufferRegion dudeMatrixRegion;
	DirectX::XMFLOAT4X4 dudeMatrix;
	pDevice->CreateDynamicBufferRegion(sizeof(DirectX::XMFLOAT4X4), NULL, &dudeMatrixRegion);
	DirectX::XMStoreFloat4x4(&dudeMatrix, DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f),
		DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(5.0f, 5.0f, 5.0f), DirectX::XMMatrixRotationRollPitchYaw(-1.5708f, 0.0f, 0.0f))));

	DirectX::XMFLOAT4X4 view;
	DirectX::XMStoreFloat4x4(&view,
		DirectX::XMMatrixLookAtLH(
			{ -5.0f, 20.0f, -5.0f },
			{ 32.0f, 0.0f, 32.0f },
			{ 0.0f, 1.0f, 0.0f }));
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMStoreFloat4x4(&projection,
		DirectX::XMMatrixPerspectiveFovLH(
			1.5708f, 1280 / (float)720, 0.1f, 8000.0f));
	XMStoreFloat4x4(&projection, p_camera->projectionMatrix);
	BufferRegion projRegion;

	BufferRegion viewRegion;

	// Per Frame Heap
	pDevice->CreateDynamicBufferRegion(
		sizeof(view),
		NULL,
		&viewRegion);

	pDevice->CreateStaticBufferRegion(
		sizeof(projection),
		NULL,
		&projRegion);

	pContext->VSSetConstantBuffer(1, viewRegion);
	pContext->VSSetConstantBuffer(2, projRegion);



	pContext->CopyDataToRegion(
		&projection,
		sizeof(projection),
		projRegion);

	BufferRegion indexRegion;
	int* indices = hexMesh.GetIndexVector()->data();

	pDevice->CreateIndexBufferRegion(
		sizeof(int) * hexMesh.GetIndexVector()->size(),
		NULL,
		&indexRegion
	);
	pContext->CopyDataToRegion(
		indices,
		sizeof(int) * hexMesh.GetIndexVector()->size(),
		indexRegion);

	
	BufferRegion dudeMeshIndexRegion;
	indices = dudeMesh.GetIndexVector()->data();
	pDevice->CreateIndexBufferRegion(
		sizeof(int)* dudeMesh.GetIndexVector()->size(),
		NULL,
		&dudeMeshIndexRegion
	);
	pContext->CopyDataToRegion(
		indices,
		sizeof(int)* dudeMesh.GetIndexVector()->size(),
		dudeMeshIndexRegion);

	pContext->UploadStaticDataToGPU();
	pContext->UploadIndexBuffersToGPU();

	pWindow->Show();

	LARGE_INTEGER starting_time, ending_time, elapsed_microseconds;
	LARGE_INTEGER freq;

	QueryPerformanceFrequency(&freq);

	pContext->CopyDataToRegion(
		&worldMatrices,
		sizeof(DirectX::XMFLOAT4X4) * 1024,
		worldMatrixBufferRegion);

	float rotation = 0.0f;
	
	pContext->CopyDataToRegion(
		&dudeMatrix,
		sizeof(DirectX::XMFLOAT4X4),
		dudeMatrixRegion);

	while (pWindow->IsOpen())
	{
		if (!pWindow->Update())
		{
			//MouseComponent* p_mouse = (MouseComponent*)ecs.getAllComponentsOfType(MouseComponent::typeID).next();
			//cout << "x: " << p_mouse->diffx << endl;
			//cout << "y: " << p_mouse->diffy << endl;

			DirectX::XMStoreFloat4x4(&dudeMatrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&dudeMatrix), DirectX::XMMatrixTranslation(rotation / 10.0f, rotation / 20.0f, rotation / 10.0f)));
			pContext->CopyDataToRegion(
				&dudeMatrix,
				sizeof(DirectX::XMFLOAT4X4),
				dudeMatrixRegion);
			QueryPerformanceCounter(&starting_time);

			// -----------------------------------------
			pContext->ClearRenderTarget(backBuffer, 34.0f / 255.f, 128.0f / 255.f, 178.0f / 255.f);
			pContext->ClearDepth(depthBuffer);
			pContext->SetRenderTarget(backBuffer, depthBuffer);

			XMStoreFloat4x4(&view, p_camera->viewMatrix);
			pContext->CopyDataToRegion(
				&view,
				sizeof(view),
				viewRegion);
			pContext->UploadDynamicDataToGPU();
			pContext->SetGraphicsPipeline(pPipeline);
			pContext->VSSetConstantBuffer(0, worldMatrixBufferRegion);
			pContext->DrawIndexedInstance(1024, 0, indexRegion, hexMeshBuffReg);

			pContext->SetGraphicsPipeline(pPipeline2);
			pContext->VSSetConstantBuffer(0, dudeMatrixRegion);
			pContext->DrawIndexedInstance(1, 0, dudeMeshIndexRegion, dudeMeshBuffReg);
			pWindow->Present();
			// -----------------------------------------
			QueryPerformanceCounter(&ending_time);
			elapsed_microseconds.QuadPart = ending_time.QuadPart - starting_time.QuadPart;
			elapsed_microseconds.QuadPart *= 1000000;
			elapsed_microseconds.QuadPart /= freq.QuadPart;
			ecs.update((float)(elapsed_microseconds.QuadPart) / 1000000.0f);
			rotation += elapsed_microseconds.QuadPart / 1000000.0f;
		}

	}
	pDevice->DeletePipeline(pPipeline);
	graphics::DeleteDeviceInterface(pDevice);
	
}