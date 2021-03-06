
/*
	256 MB RAM

	256 MB VRAM vid 720p
	512 MB VRAM vid 1080p

	1 GB HDD

	AVG FPS: 30 (MINIMUM)
*/

#include "../includes/graphics/DeviceInterface.h"

#ifdef _DEBUG
	#pragma comment(lib, "GraphicsEngine_d.lib")
#else
	#pragma comment(lib, "GraphicsEngine.lib")
#endif // DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <string>
#include <vector>
#include <DirectXMath.h>

#define MESHES_X_AXIS (120)
#define MESHES_Y_AXIS (32)
#define MAXIMUM_MESHES_TO_DRAW (MESHES_X_AXIS * MESHES_Y_AXIS)//(ecs::systems::compCount)

const std::string gVertexShader = R"(

struct PerObjectData
{
	float4x4 World;
};

cbuffer gTransformation : register (b0)
{
	PerObjectData gMesh[3];
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

	float4x4 wvp = mul(gPerspective, mul(gView, gMesh[instance].World));

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

// Structs: must be variables on the stack
// classes: must be pointers retrieved from 'DeviceInterface'

void AddTransformation(
	std::vector<DirectX::XMFLOAT4X4>& transformations,
	const float x,
	const float y,
	const float z)
{
	DirectX::XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, DirectX::XMMatrixTranslation(x, y, z));
	transformations.push_back(temp);
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	using namespace DirectX;
	using namespace graphics;

	UINT 
		clientWidth		= 1280, 
		clientHeight	= 720;

	DeviceInterface* pDevice		= NULL;
	GraphicsPipeline* pPipeline		= NULL;

	CreateDeviceInterface(&pDevice);
	RenderContext* pContext = pDevice->GetRenderContext();

	RenderTarget backBuffer;
	DepthBuffer depthBuffer;
	PresentWindow wnd;

	pDevice->CreatePresentWindow(
		clientWidth, 
		clientHeight, 
		"D3D11", 
		&backBuffer, 	
		&wnd);

	pDevice->CreateDepthBuffer(
		clientWidth, 
		clientHeight,
		&depthBuffer);

	pDevice->CreateGraphicsPipeline(
		gVertexShader,
		gPixelShader,
		&pPipeline);

	pContext->SetViewport(0, 0, clientWidth, clientHeight);
	pContext->SetGraphicsPipeline(pPipeline);


	// Create Mesh

	struct float3
	{
		float x, y, z;
	};

	struct float2
	{
		float x, y;
	};

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

	BufferRegion meshes[2];	// mesh

	pDevice->CreateVertexBufferRegion(3, t, NULL, uv, NULL, NULL,  &meshes[0]);
	pDevice->CreateVertexBufferRegion(6, q, NULL, NULL, NULL, NULL, &meshes[1]);

	pContext->UploadBufferToGPU(BUFFER_UPLOAD_VERTEX_DATA);

	// Create Matrices

	std::vector<XMFLOAT4X4> transformation;
	for (UINT x = 0; x < MESHES_X_AXIS; x++)
	{
		for (UINT y = 0; y < MESHES_Y_AXIS; y++)
		{
			AddTransformation(
				transformation,
				x * 2.0f,
				y * 2.0f,
				0);
		}
	}


	XMFLOAT4X4 view;
	XMStoreFloat4x4(&view,
		XMMatrixLookToLH(
			{ 5.5f, 0.0f, -20.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f }));

	XMFLOAT4X4 projection;
	XMStoreFloat4x4(&projection,
		XMMatrixPerspectiveFovLH(
			3.14f / 2.0f, clientWidth / (float)clientHeight, 0.1f, 800.0f));


	BufferRegion projRegion;
	BufferRegion viewRegion;
	BufferRegion buffer0;

	// Per Frame Heap
	pDevice->CreateDynamicBufferRegion(
		sizeof(view),
		NULL,
		&viewRegion);

	pDevice->CreateDynamicBufferRegion(
		sizeof(XMFLOAT4X4) * MAXIMUM_MESHES_TO_DRAW,
		NULL,
		&buffer0);

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

	pContext->UploadBufferToGPU(BUFFER_UPLOAD_STATIC_DATA);

	wnd.Show();
	while (wnd.IsOpen())
	{
		if (!wnd.Update())
		{
			// --- RENDER MANAGER ---
			pContext->ClearDepth(depthBuffer);
			pContext->ClearRenderTarget(backBuffer, 0.0f, 0.0f, 0.0f);

			pContext->SetRenderTarget(backBuffer, depthBuffer);

			// Copy Data to CPU Buffer (View Matrix)
			pContext->CopyDataToRegion( 
				&view,
				sizeof(view),
				viewRegion);

			// Copy Data to CPU Buffer (World Matrices)
			pContext->CopyDataToRegion(
				transformation.data(),
				MAXIMUM_MESHES_TO_DRAW * sizeof(XMFLOAT4X4),
				buffer0);
			 
			// Upload All Data to GPU
			pContext->UploadBufferToGPU(BUFFER_UPLOAD_DYNAMIC_DATA);

			UINT at		= buffer0.DataLocation;
			UINT end	= buffer0.DataLocation + buffer0.DataCount;

			BufferRegion r	= buffer0;
			r.DataCount		= RenderContext::CB_MAX_BYTES_PER_BIND;


			// --- DRAW MANAGER ---
			//UINT meshIndex = 0;
			//std::map<UINT, UINT> meshCount = transformation.size();
			//while (at < end && meshIndex < 2)
			//{
			//	r.DataLocation = at;							// Set active region (65536 bytes per draw)
			//	pContext->VSSetConstantBuffer(0, r);			// Apply active Region
			//	
			//	UINT drawn = 0;
			//	while (drawn < 1024 && meshIndex < meshCount.size())
			//	{
			//		UINT count = meshCount[meshIndex];

			//		if (count + drawn > 1024)
			//		{
			//			count = 1024 - drawn;
			//		}

			//		meshCount[meshIndex] -= count;

			//		pContext->DrawInstanced(count, drawn, meshes[meshIndex]);		// Draw (with mesh[1])
			//		drawn += count;

			//		if (meshCount[meshIndex] == 0)
			//		{
			//			meshIndex++;
			//		}
			//	}
			//	
			//	at += RenderContext::CB_MAX_BYTES_PER_BIND;									// Increment active region
			//}

			// Present
			wnd.Present();
		}
	}

	pDevice->DeleteRenderTarget(backBuffer);
	pDevice->DeleteDepthBuffer(depthBuffer);
	pDevice->DeleteGraphicsPipeline(pPipeline);
	pDevice->DeletePresentWindow(wnd);

	DeleteDeviceInterface(pDevice);

	return 0;
}