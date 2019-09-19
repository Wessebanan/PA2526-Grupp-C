#include <ecs.h>
#include <Mesh.h>
#include <DeviceInterface.h>
#include <UtilityFunctions.h>
#include <CameraFunctions.h>
#include <DebugInfo.h>
#include "Shaders.h"

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
	ecs::EntityComponentSystem ecs;
	CameraFunctions::CreateDevCamera(ecs);
	CameraFunctions::CreateCameraSystems(ecs);


	ecs::ComponentIterator iter = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	ecs::BaseComponent* baseComp = iter.next();
	ecs::components::CameraComponent* camera = static_cast<ecs::components::CameraComponent*>(baseComp);
	size_t system_count = ecs.getTotalSystemCount();





	//camera.
	DeviceInterface* pDevice;
	PresentWindow* pWindow;
	GraphicsPipeline* pPipeline;
	CreateDeviceInterface(&pDevice);
	ModelLoader::Mesh hexMesh;
	BufferRegion meshBuffReg;

	CreateDeviceInterface(&pDevice);
	RenderContext* pContext = pDevice->QueryRenderContext();
	Texture2DView* pBackBuffer = pDevice->QueryBackBuffer();

	pDevice->CreatePresentWindow(1280, 720, "D3D11", &pWindow);
	
	pDevice->CreatePipeline(gVertexShader, gPixelShader, &pPipeline);

	
	pContext->SetViewport(0, 0, 1280, 720);
	pContext->SetGraphicsPipeline(pPipeline);

	HRESULT hr = hexMesh.LoadFBX("Models\\hexTile3.fbx");
	std::vector<DirectX::XMFLOAT3>* pVertices = hexMesh.GetVertexPositionVector();

	std::vector<DirectX::XMFLOAT3>* test = hexMesh.GetNormalVector();
	if (!hexMesh.HasUVs())
	{
		pDevice->CreateMeshRegion(pVertices->size(), pVertices->data(), hexMesh.GetNormalVector()->data(), NULL, &meshBuffReg);
	}
	else
	{
		pDevice->CreateMeshRegion(pVertices->size(), pVertices->data(), hexMesh.GetNormalVector()->data(), hexMesh.GetUVVector()->data(), &meshBuffReg);
	}

	ModelLoader::Mesh dudeMesh;

	pContext->UploadMeshesToGPU();

	BufferRegion worldMatrixBufferRegion;
	pDevice->CreateDynamicBufferRegion(sizeof(DirectX::XMFLOAT4X4) * 1024, &worldMatrixBufferRegion);
	
	
	DirectX::XMFLOAT4X4 world(1.0f, 0.0f, 0.0f, 0.0f,
							  0.0f, 1.0f, 0.0f, 0.0f,
							  0.0f, 0.0f, 1.0f, 0.0f,
							  0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMStoreFloat4x4(&world,
		DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(2.0f, 2.0f, 2.0f), DirectX::XMMatrixRotationRollPitchYaw(-1.5708f, 0.0f, 0.0f)));

	DirectX::XMFLOAT4X4 worldMatrices[1024];
	worldMatrices[0] = world;
	//worldMatrices[1] = world2;
	//worldMatrices[2] = world3;
	for (int i = 0; i < 32; ++i)
	{
		for (int j = 0; j < 32; ++j)
		{
			if(i % 2 == 0)
				DirectX::XMStoreFloat4x4(&worldMatrices[i* 32 + j], DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&world), DirectX::XMMatrixTranslation(j * 3.2f, 0.0f, i * 3.2f)));
			else
				DirectX::XMStoreFloat4x4(&worldMatrices[i * 32 + j], DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&world), DirectX::XMMatrixTranslation(j * 3.2f + 1.5f, 0.0f, i * 3.1f)));
		}
	}
	DirectX::XMFLOAT4X4 view;
	DirectX::XMStoreFloat4x4(&view,
		DirectX::XMMatrixLookAtLH(
			{ 32.0f, 20.0f, -5.0f },
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
		&viewRegion);

	pDevice->CreateStaticBufferRegion(
		sizeof(projection),
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
		&indexRegion
	);


	pContext->CopyDataToRegion(
		indices,
		sizeof(int) * hexMesh.GetIndexVector()->size(),
		indexRegion);

	pContext->UploadToGPU(BUFFER_CONSTANT_STATIC);
	pContext->UploadToGPU(BUFFER_INDEX);

	pWindow->Show();

	LARGE_INTEGER starting_time, ending_time, elapsed_microseconds;
	LARGE_INTEGER freq;

	QueryPerformanceFrequency(&freq);

	pContext->CopyDataToRegion(
		&worldMatrices,
		sizeof(DirectX::XMFLOAT4X4) * 1024,
		worldMatrixBufferRegion);
	
	while (pWindow->IsOpen())
	{
		if (!pWindow->Update())
		{

			QueryPerformanceCounter(&starting_time);
			pContext->ClearRenderTarget(pBackBuffer, 34.0f / 255.f, 128.0f / 255.f, 178.0f / 255.f);
			pContext->SetRenderTarget(pBackBuffer);

			pContext->CopyDataToRegion(
				&view,
				sizeof(view),
				viewRegion);

			pContext->UploadToGPU(BUFFER_CONSTANT_DYNAMIC);
			pContext->VSSetConstantBuffer(0, worldMatrixBufferRegion);
			//pContext->DrawInstanced(1, 0, meshBuffReg);
			pContext->DrawIndexedInstance(1024, 0, indexRegion, meshBuffReg);
			pWindow->Present();
			QueryPerformanceCounter(&ending_time);
			elapsed_microseconds.QuadPart = ending_time.QuadPart - starting_time.QuadPart;
			elapsed_microseconds.QuadPart *= 1000000;
			elapsed_microseconds.QuadPart /= freq.QuadPart;
			ecs.update((float)(elapsed_microseconds.QuadPart) / 1000000.0f);
		}

	}
	pDevice->DeletePipeline(pPipeline);
	pDevice->Release();
	
}