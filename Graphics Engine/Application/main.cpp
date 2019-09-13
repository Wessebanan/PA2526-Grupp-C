
/*
	256 MB RAM

	256 MB VRAM vid 720p
	512 MB VRAM vid 1080p

	1 GB HDD

	AVG FPS: 30 (MINIMUM)
*/

#include "../includes/DeviceInterface.h"
#include "../../ECS/plainECS/includes/ecs.h"

#pragma comment(lib, "plainECS.lib")

#ifdef _DEBUG
	#pragma comment(lib, "GraphicsEngine_d.lib")
#else
	#pragma comment(lib, "GraphicsEngine.lib")
#endif // DEBUG


#include <string>
#include <vector>
#include <DirectXMath.h>

#include "../includes/GraphicsSystems.h"
#include "../includes/GraphicsComponents.h"

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

constexpr unsigned int compCount = 4000;

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
	using namespace DirectX;
	using namespace graphics;
	using namespace ecs;


	systems::MeshRenderSystemData mrData = { 0 };
	EntityComponentSystem myECS;
	{
		ecs::ECSDesc ecsDesc;
		ecs::CompTypeMemDesc ecsMemDesc[] =
		{
			{ components::WorldComponent::typeID, components::WorldComponent::size, compCount},
			{ components::MeshComponent::typeID, components::MeshComponent::size, compCount},
		};
		ecsDesc.compTypeCount = 2;
		ecsDesc.compTypeMemDescs = ecsMemDesc;
		ecsDesc.systemLayerCount = 10;
		myECS.initialize(ecsDesc);
	}

	systems::MeshRenderSystem* sys = myECS.createSystem<systems::MeshRenderSystem>();
	sys->m_pData = &mrData;

	UINT 
		clientWidth		= 1280, 
		clientHeight	= 720;

	DeviceInterface* pDevice		= NULL;
	GraphicsPipeline* pPipeline		= NULL;
	PresentWindow* pWindow			= NULL;

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

	BufferRegion meshes[2];	// mesh

	pDevice->CreateMeshRegion(3, t, NULL, uv, &meshes[0]);
	pDevice->CreateMeshRegion(6, q, NULL, NULL, &meshes[1]);

	pContext->UploadMeshesToGPU();

	// Create Matrices

	std::vector<XMFLOAT4X4> transformation;
	for (UINT x = 0; x < 100; x++)
	{
		for (UINT y = 0; y < 40; y++)
		{
			AddTransformation(
				transformation,
				x * 2.0f,
				y * 2.0f,
				0);
		}
	}

	components::WorldComponent wc[compCount];
	components::MeshComponent mc[2];

	mc[0].MeshLocation = meshes[0];
	mc[1].MeshLocation = meshes[1];

	for (UINT i = 0; i < compCount; i++)
	{
		wc[i].WorldMatrix = transformation[i];
	}

	UINT half = compCount / 2;
	for (UINT i = 0; i < half; i++)
	{
		myECS.createEntity(wc[i], mc[0]);
	}
	for (UINT i = 0; i < half; i++)
	{
		UINT index = i + half;
		myECS.createEntity(wc[index], mc[1]);
	}

	myECS.update(0); 


	XMFLOAT4X4 view;
	XMStoreFloat4x4(&view,
		XMMatrixLookToLH(
			{ 5.5f, 5.0f, -20.0f },
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
		&viewRegion);

	pDevice->CreateDynamicBufferRegion(
		sizeof(XMFLOAT4X4) * (UINT)mrData.m_matrices.size(),
		&buffer0);

	pDevice->CreateStaticBufferRegion(
		sizeof(projection),
		&projRegion);

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
				mrData.m_matrices.data(),
				mrData.m_matrices.size() * sizeof(XMFLOAT4X4),
				buffer0);

			// Upload All Data to GPU
			pContext->UploadToGPU(BUFFER_CONSTANT_DYNAMIC);

			UINT at = buffer0.DataLocation;
			UINT end = buffer0.DataLocation + buffer0.DataCount;

			BufferRegion r = buffer0;
			r.DataCount = 65536;

			while (at < end)
			{
				r.DataLocation = at;							// Set active region
				pContext->VSSetConstantBuffer(0, r);			// Apply active Region
				pContext->DrawInstanced(512, 0, meshes[1]);		// Draw
				pContext->DrawInstanced(512, 512, meshes[0]);	// Draw
				at += 65536;									// Increment active region
			}


			// Presenet
			pWindow->Present();
		}
	}

	pDevice->DeletePipeline(pPipeline);
	pDevice->Release();

	return 0;
}