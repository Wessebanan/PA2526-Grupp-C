
/*
	256 MB RAM
	
	256 MB VRAM vid 720p
	512 MB VRAM vid 1080p

	1 GB HDD

	AVG FPS: 30 (MINIMUM)
*/

#include "../includes/DeviceInterface.h"
#include <string>
#include <DirectXMath.h>

const std::string gVertexShader = R"(

cbuffer gTransformation : register (b0)
{
	float4x4 gWorld[3];
};

cbuffer gCam : register (b1)
{
	float4x4 gView;
};

cbuffer gPer : register (b2)
{
	float4x4 gPerspective;
};

struct VSOUT
{
	float4 pos	: SV_POSITION;
	float2 uv	: UV0;
};

VSOUT main(
	float3 pos : POSITION0, 
	float2 uv : UV0,
	uint instance : INDEX0)
{
	VSOUT output;

	float4x4 wvp = mul(gPerspective, mul(gView, gWorld[instance]));

	output.pos	= mul(wvp, float4(pos, 1.0f));
	output.uv	= uv;
	
	return output;
}	

)";

const std::string gPixelShader = R"(

struct PSIN
{
	float4 pos	: SV_POSITION;
	float2 uv	: UV0;
};

float4 main(PSIN input) : SV_TARGET
{ 
	return float4(input.uv.x, 1.0f, input.uv.y, 1.0f);
}	

)";

int main()
{
	using namespace DirectX;

	UINT clientWidth = 1280, clientHeight = 720;

	DeviceInterface* pDevice	= NULL;
	GraphicsPipeline* pPipeline = NULL;
	MeshHeap* pMesh				= NULL;
	PresentWindow* pWindow		= NULL;

	CreateDeviceInterface(&pDevice);
	RenderContext* pContext = pDevice->QueryRenderContext();
	Texture2DView* pBackBuffer = pDevice->QueryBackBuffer();

	pDevice->CreatePresentWindow(clientWidth, clientHeight, "D3D11", &pWindow);

	pDevice->CreatePipeline(
		gVertexShader,
		gPixelShader,
		&pPipeline);

	pContext->SetViewport(0, 0, clientWidth, clientHeight);
	pContext->SetGraphicsPipeline(pPipeline);


	// Create Mesh

	float3 t[3] = {
		-1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
	};

	float2 uv[3] = {
		0.0f, 1.0f,
		0.5f, 0.0f,
		1.0f, 1.0f,
	};

	float3 q[6] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,

		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	BufferRegion triangle;
	BufferRegion quad;

	pDevice->CreateMeshRegion(3, t, NULL, uv, &triangle);
	pDevice->CreateMeshRegion(6, q, NULL, NULL, &quad);

	pContext->UploadMeshesToGPU();

	// Create Matrices

	XMFLOAT4X4 transformation0[10];
	XMStoreFloat4x4(&transformation0[0], XMMatrixTranslation(-1.0f, 0.0f, 0.0f));
	XMStoreFloat4x4(&transformation0[1], XMMatrixTranslation(0.0f, 2.0f, 0.0f));
	XMStoreFloat4x4(&transformation0[2], XMMatrixTranslation(1.0f, 0.0f, 0.0f));

	XMStoreFloat4x4(&transformation0[3], XMMatrixTranslation(-5.0f, 5.0f, 0.0f));
	XMStoreFloat4x4(&transformation0[4], XMMatrixTranslation(0.0f, 5.0f, 0.0f));
	XMStoreFloat4x4(&transformation0[5], XMMatrixTranslation(5.0f, 5.0f, 0.0f));

	XMStoreFloat4x4(&transformation0[6], XMMatrixTranslation(-5.0f, 7.0f, 0.0f));
	XMStoreFloat4x4(&transformation0[7], XMMatrixTranslation(0.0f, 7.0f, 0.0f));
	XMStoreFloat4x4(&transformation0[8], XMMatrixTranslation(5.0f, 7.0f, 0.0f));

	XMStoreFloat4x4(&transformation0[9], XMMatrixTranslation(10.0f, 7.0f, 0.0f));

	XMFLOAT4X4 view;
	XMStoreFloat4x4(&view,
		XMMatrixLookToLH(
			{ 5.5f, 5.0f, -10.0f },
			{ -0.5f, 0.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f }));

	XMFLOAT4X4 projection;
	XMStoreFloat4x4(&projection,
		XMMatrixPerspectiveFovLH(
			3.14f / 2.0f, clientWidth / (float)clientHeight, 0.1f, 100.0f));


	BufferRegion projRegion;
	BufferRegion viewRegion;
	BufferRegion buffer0;

	// Per Frame Heap
	pDevice->CreateBufferRegion(
		BUFFER_CONSTANT_DYNAMIC,
		sizeof(view), 
		&viewRegion);

	pDevice->CreateBufferRegion(
		BUFFER_CONSTANT_DYNAMIC,
		sizeof(transformation0),
		&buffer0);

	pDevice->CreateBufferRegion(
		BUFFER_CONSTANT_STATIC,
		sizeof(projection),
		&projRegion);

	pContext->VSSetConstantBuffer(0, buffer0);
	pContext->VSSetConstantBuffer(1, viewRegion);
	pContext->VSSetConstantBuffer(2, projRegion);

	pContext->CopyDataToRegion(
		&projection,
		sizeof(projection),
		projRegion);

	pContext->UploadToGPU(BUFFER_CONSTANT_STATIC);

	pWindow->Show();
	while (pWindow->IsOpen())
	{
		if (!pWindow->Update())
		{
			pContext->ClearRenderTarget(pBackBuffer, 0.0f, 0.0f, 0.0f);
			pContext->SetRenderTarget(pBackBuffer);

			// Copy Data to CPU Buffer (View Matrix)
			pContext->CopyDataToRegion(
				&view, 
				sizeof(view),
				viewRegion);

			// Copy Data to CPU Buffer (World Matrices)
			pContext->CopyDataToRegion(
				transformation0, 
				sizeof(transformation0),
				buffer0);

			// Upload All Data to GPU
			pContext->UploadToGPU(BUFFER_CONSTANT_DYNAMIC);

			// Draw
			pContext->DrawInstanced(0, 3, triangle);
			pContext->DrawInstanced(3, 3, quad);
			pContext->DrawInstanced(6, 4, triangle);

			// Presenet
			pWindow->Present();
		}
	}

	pDevice->DeletePipeline(pPipeline);
	pDevice->Release();

	return 0;
}