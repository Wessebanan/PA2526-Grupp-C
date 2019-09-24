#include <ecs.h>
#include "InitInputHandler.h"
#include <Mesh.h>
#include <DeviceInterface.h>
#include <UtilityFunctions.h>
#include <CameraFunctions.h>
#include <GridFunctions.h>
#include <DebugInfo.h>
#include "Shaders.h"

#define GRID_WIDTH 32
#define GRID_HEIGHT 25
#define GRID_HEX_SIZE 8

namespace DInfo
{
	namespace Graphics
	{
		DINFO_ADD_PARAM(FPS, int, 0);
	}
}

//#pragma comment (lib, "GraphicsEngine_d")
using namespace graphics;
int main()
{
	ecs::CompTypeMemDesc ecsMemDesc[] = {
		{ TileComponent::typeID, TileComponent::size, GRID_WIDTH * GRID_HEIGHT },
		{ TransformComponent::typeID, TransformComponent::size, GRID_WIDTH * GRID_HEIGHT },
	};

	ecs::ECSDesc ecsDesc;
	ecsDesc.compTypeCount = 2;
	ecsDesc.compTypeMemDescs = ecsMemDesc;
	ecsDesc.systemLayerCount = 10;

	ecs::EntityComponentSystem ecs;
	ecs.initialize(ecsDesc);
	
	WebConnection web;
	InputBackend* inp;
	//inp = new InputBackend;
	//initInputECS(ecs,inp);

	//CameraFunctions::CreateDevCamera(ecs);
	//CameraFunctions::CreateCameraSystems(ecs);


	//ecs::ComponentIterator iter = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	//ecs::BaseComponent* baseComp = iter.next();
	//ecs::components::CameraComponent* camera = static_cast<ecs::components::CameraComponent*>(baseComp);
	//size_t system_count = ecs.getTotalSystemCount();




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
	pDevice->CreateDynamicBufferRegion(sizeof(DirectX::XMFLOAT4X4) * GRID_HEIGHT * GRID_WIDTH, NULL, &worldMatrixBufferRegion);
	
	
	DirectX::XMFLOAT4X4 world(1.0f, 0.0f, 0.0f, 0.0f,
							  0.0f, 1.0f, 0.0f, 0.0f,
							  0.0f, 0.0f, 1.0f, 0.0f,
							  0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMStoreFloat4x4(&world,
		DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(GRID_HEX_SIZE, GRID_HEX_SIZE, GRID_HEX_SIZE), DirectX::XMMatrixRotationRollPitchYaw(-1.5708f, 0.0f, -1.5708f)));

	DirectX::XMFLOAT4X4 worldMatrices[GRID_HEIGHT * GRID_WIDTH];
	
	GridFunctions::CreateGrid(ecs, GRID_WIDTH, GRID_HEIGHT, GRID_HEX_SIZE);
	ecs::ComponentIterator comp_iter = ecs.getAllComponentsOfType(ecs::components::TileComponent::typeID);
	ecs::BaseComponent* p;
	ecs::components::TransformComponent* pTransform;
	DirectX::XMFLOAT3 tile_positions[GRID_HEIGHT * GRID_WIDTH];
	int counter = 0;
	while (p = comp_iter.next())
	{
		pTransform = static_cast<ecs::components::TransformComponent*>(ecs.getComponentFromEntity(ecs::components::TransformComponent::typeID, p->getEntityID()));
		tile_positions[counter] = pTransform->position;
		counter++;
	}
	
	for (int i = 0; i < GRID_HEIGHT; ++i)
	{
		for (int j = 0; j < GRID_WIDTH; ++j)
		{
			if (i % 2 == 0) {

				DirectX::XMStoreFloat4x4(&worldMatrices[i* GRID_HEIGHT + j], DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&world), DirectX::XMMatrixTranslation(j * 3.2f, 0.0f, i * 3.2f)));

			}
			else {
				DirectX::XMStoreFloat4x4(&worldMatrices[i * GRID_HEIGHT + j], DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&world), DirectX::XMMatrixTranslation(j * 3.2f + 1.5f, 0.0f, i * 3.1f)));
			}
			DirectX::XMStoreFloat4x4(&worldMatrices[i * GRID_HEIGHT + j], DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&world), DirectX::XMMatrixTranslation(tile_positions[i * GRID_HEIGHT + j].x, tile_positions[i * GRID_HEIGHT + j].y, tile_positions[i * GRID_HEIGHT + j].z)));
		}
	}

	BufferRegion dudeMatrixRegion;
	DirectX::XMFLOAT4X4 dudeMatrix;
	pDevice->CreateDynamicBufferRegion(sizeof(DirectX::XMFLOAT4X4), NULL, &dudeMatrixRegion);
	DirectX::XMStoreFloat4x4(&dudeMatrix, DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f),
		DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(2.5f, 2.5f, 2.5f), DirectX::XMMatrixRotationRollPitchYaw(-1.5708f, 0.0f, 0.0f))));

	DirectX::XMFLOAT4X4 view;
	DirectX::XMStoreFloat4x4(&view,
		DirectX::XMMatrixLookAtLH(
			{ -5.0f, 60.0f, -5.0f },
			{ 32.0f, 0.0f, 32.0f },
			{ 0.0f, 1.0f, 0.0f }));
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMStoreFloat4x4(&projection,
		DirectX::XMMatrixPerspectiveFovLH(
			1.5708f, 1280 / (float)720, 0.1f, 8000.0f));
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
		sizeof(DirectX::XMFLOAT4X4) * GRID_HEIGHT * GRID_WIDTH,
		worldMatrixBufferRegion);

	float rotation = 0.0f;
	
	pContext->CopyDataToRegion(
		&dudeMatrix,
		sizeof(DirectX::XMFLOAT4X4),
		dudeMatrixRegion);
	
	DirectX::XMStoreFloat4x4(&dudeMatrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&dudeMatrix), DirectX::XMMatrixTranslation(0.0f, 30.0f, 0.0f)));
	while (pWindow->IsOpen())
	{
		if (!pWindow->Update())
		{
			ecs::ComponentIterator iter2 = ecs.getAllComponentsOfType(ecs::components::UserButtonComponent::typeID);
			ecs::BaseComponent* user_button;
			bool user0Button0;



			int h_u0b0 = web.getPlayerButton(0);
			if (h_u0b0 == 0)
			{
				DirectX::XMStoreFloat4x4(&dudeMatrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&dudeMatrix), DirectX::XMMatrixTranslation(-rotation / 1.0f, 0.0f, 0.0f)));
			}
			else if (h_u0b0 == 1)
			{
				DirectX::XMStoreFloat4x4(&dudeMatrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&dudeMatrix), DirectX::XMMatrixTranslation(0.0f, 0.0f, -rotation / 1.0f)));
			}
			else if (h_u0b0 == 2)
			{
				DirectX::XMStoreFloat4x4(&dudeMatrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&dudeMatrix), DirectX::XMMatrixTranslation(0.0f, 0.0f, rotation / 1.0f)));
			}
			else if (h_u0b0 == 3)
			{
				DirectX::XMStoreFloat4x4(&dudeMatrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&dudeMatrix), DirectX::XMMatrixTranslation(rotation / 1.0f, 0.0f, 0.0f)));
			}


			pContext->CopyDataToRegion(
				&dudeMatrix,
				sizeof(DirectX::XMFLOAT4X4),
				dudeMatrixRegion);
			QueryPerformanceCounter(&starting_time);

			// -----------------------------------------
			pContext->ClearRenderTarget(backBuffer, 34.0f / 255.f, 128.0f / 255.f, 178.0f / 255.f);
			pContext->ClearDepth(depthBuffer);
			pContext->SetRenderTarget(backBuffer, depthBuffer);

			pContext->CopyDataToRegion(
				&view,
				sizeof(view),
				viewRegion);
			pContext->UploadDynamicDataToGPU();

			pContext->SetGraphicsPipeline(pPipeline);
			pContext->VSSetConstantBuffer(0, worldMatrixBufferRegion);
			pContext->DrawIndexedInstance(GRID_HEIGHT * GRID_WIDTH, 0, indexRegion, hexMeshBuffReg);

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
			char buffer[100];
			sprintf_s(buffer, "Fps: %f\n", 1000.0f / ((float)(elapsed_microseconds.QuadPart) / 1000.0f));
			OutputDebugStringA(buffer);
			rotation = elapsed_microseconds.QuadPart / 500000.0f;
		}

	}
	pDevice->DeletePipeline(pPipeline);
	graphics::DeleteDeviceInterface(pDevice);
	//delete inp;
	
}