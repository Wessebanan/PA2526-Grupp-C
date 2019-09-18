#include <ecs.h>
#include <Mesh.h>
#include <DeviceInterface.h>
#include "Shaders.h"

#pragma comment (lib, "GraphicsEngine_d")
using namespace graphics;
int main()
{
	DeviceInterface* pDevice;
	PresentWindow* pWindow;
	GraphicsPipeline* pPipeline;
	CreateDeviceInterface(&pDevice);
	ModelLoader::Mesh testMesh;
	BufferRegion meshBuffReg;

	CreateDeviceInterface(&pDevice);
	RenderContext* pContext = pDevice->QueryRenderContext();
	Texture2DView* pBackBuffer = pDevice->QueryBackBuffer();

	pDevice->CreatePresentWindow(1280, 720, "D3D11", &pWindow);
	
	pDevice->CreatePipeline(gVertexShader, gPixelShader, &pPipeline);

	
	pContext->SetViewport(0, 0, 1280, 720);
	pContext->SetGraphicsPipeline(pPipeline);

	HRESULT hr = testMesh.LoadFBX("C:\\Users\\magno\\source\\repos\\PA2526-Grupp-C\\Sprint1\\x64\\Debug\\cat.fbx");
	std::vector<DirectX::XMFLOAT3>* pVertices = testMesh.GetVertexPositionVector();

	std::vector<DirectX::XMFLOAT3>* test = testMesh.GetNormalVector();
	if (!testMesh.HasUVs())
	{
		pDevice->CreateMeshRegion(pVertices->size(), pVertices->data(), testMesh.GetNormalVector()->data(), NULL, &meshBuffReg);
	}
	else
	{
		pDevice->CreateMeshRegion(pVertices->size(), pVertices->data(), testMesh.GetNormalVector()->data(), testMesh.GetUVVector()->data(), &meshBuffReg);
	}
	
	pContext->UploadMeshesToGPU();

	BufferRegion worldMatrixBufferRegion;
	pDevice->CreateDynamicBufferRegion(sizeof(DirectX::XMFLOAT4X4) * 1024, &worldMatrixBufferRegion);
	
	
	DirectX::XMFLOAT4X4 world(1.0f, 0.0f, 0.0f, 0.0f,
							  0.0f, 1.0f, 0.0f, 0.0f,
							  0.0f, 0.0f, 1.0f, 0.0f,
							  0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMStoreFloat4x4(&world,
		DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(0.005f, 0.005f, 0.005f), DirectX::XMMatrixRotationRollPitchYaw(-1.5708f, 0.0f, 0.0f)));

	DirectX::XMFLOAT4X4 worldMatrices[1024];
	worldMatrices[0] = world;
	//worldMatrices[1] = world2;
	//worldMatrices[2] = world3;
	for (int i = 0; i < 32; ++i)
	{
		for (int j = 0; j < 32; ++j)
		{
			DirectX::XMStoreFloat4x4(&worldMatrices[i* 32 + j], DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&world), DirectX::XMMatrixTranslation(j * 2.5f, 0.0f, i * 3.5f)));
		}
	}
	DirectX::XMFLOAT4X4 view;
	DirectX::XMStoreFloat4x4(&view,
		DirectX::XMMatrixLookAtLH(
			{ 40.0f, 5.0f, -0.0f },
			{ 40.0f, 0.0f, 40.0f },
			{ 0.0f, 1.0f, 0.0f }));

	DirectX::XMFLOAT4X4 projection;
	DirectX::XMStoreFloat4x4(&projection,
		DirectX::XMMatrixPerspectiveFovLH(
			3.14f / 2.0f, 1280 / (float)720, 0.1f, 8000.0f));
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
	int* indices = testMesh.GetIndexVector()->data();

	pDevice->CreateIndexBufferRegion(
		sizeof(int) * testMesh.GetIndexVector()->size(),
		&indexRegion
	);


	pContext->CopyDataToRegion(
		indices,
		sizeof(int) * testMesh.GetIndexVector()->size(),
		indexRegion);

	pContext->UploadToGPU(BUFFER_CONSTANT_STATIC);
	pContext->UploadToGPU(BUFFER_INDEX);
	 
	pWindow->Show();
	while (pWindow->IsOpen())
	{
		if (!pWindow->Update())
		{
			pContext->ClearRenderTarget(pBackBuffer, 0.1f, 0.0f, 0.0f);
			pContext->SetRenderTarget(pBackBuffer);

			pContext->CopyDataToRegion(
				&view,
				sizeof(view),
				viewRegion);

			pContext->CopyDataToRegion(
				&worldMatrices,
				sizeof(DirectX::XMFLOAT4X4) * 1024,
				worldMatrixBufferRegion);

			pContext->UploadToGPU(BUFFER_CONSTANT_DYNAMIC);
			pContext->VSSetConstantBuffer(0, worldMatrixBufferRegion);
			//pContext->DrawInstanced(1, 0, meshBuffReg);
			pContext->DrawIndexedInstance(1024, 0, indexRegion, meshBuffReg);
			pWindow->Present();
		}

	}
	pDevice->DeletePipeline(pPipeline);
	pDevice->Release();
	
}