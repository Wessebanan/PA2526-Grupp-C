#include "pch.h"
#include "../includes/DeviceInterface.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <DirectXMath.h>

#ifdef _DEBUG
	#pragma comment(lib, "GraphicsEngine_d.lib")
#else
	#pragma comment(lib, "GraphicsEngine.lib")
#endif // DEBUG

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

#define MESHES_X_AXIS (120)
#define MESHES_Y_AXIS (32)
#define MAXIMUM_MESHES_TO_DRAW (MESHES_X_AXIS * MESHES_Y_AXIS)

// --- Help functions ---
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

// --- Tests ---
TEST(TestingDeviceInterface, CreationAndDeletion)
{
	graphics::DeviceInterface* pDevice;

	int result = graphics::CreateDeviceInterface(&pDevice);

	EXPECT_TRUE(pDevice);
	EXPECT_TRUE(result);

	graphics::DeleteDeviceInterface(pDevice);
}

TEST(TestingDeviceInterface, CreatePresentWindow)
{
	graphics::DeviceInterface* pDevice;
	graphics::CreateDeviceInterface(&pDevice);

	graphics::RenderTarget target;
	graphics::PresentWindow wnd;
	{
		int result = pDevice->CreatePresentWindow(
			800,
			600,
			"Test",
			&target,
			&wnd);

		EXPECT_TRUE(result);

		if (result)
		{
			pDevice->DeleteRenderTarget(target);
			pDevice->DeletePresentWindow(wnd);
		}
	}
	graphics::DeleteDeviceInterface(pDevice);
}

TEST(TestingDeviceInterface, CreateDepthBuffer)
{
	graphics::DeviceInterface* pDevice;
	graphics::CreateDeviceInterface(&pDevice);

	graphics::DepthBuffer depthBuffer;
	{
		int result = pDevice->CreateDepthBuffer(800, 600, &depthBuffer);

		EXPECT_TRUE(result);

		if (result)
		{
			pDevice->DeleteDepthBuffer(depthBuffer);
		}
	}
	graphics::DeleteDeviceInterface(pDevice);
}

TEST(TestingDeviceInterface, CreateGraphicsPipeline)
{
	graphics::DeviceInterface* pDevice;
	graphics::CreateDeviceInterface(&pDevice);

	graphics::GraphicsPipeline* pPipeline;
	{
		int result = pDevice->CreateGraphicsPipeline(
			gVertexShader, 
			gPixelShader, 
			&pPipeline);

		EXPECT_TRUE(result);

		if (result)
		{
			pDevice->DeleteGraphicsPipeline(pPipeline);
		}
	}
	graphics::DeleteDeviceInterface(pDevice);
}

TEST(TestingDeviceInterface, CreateVertexBufferRegion)
{
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

	graphics::DeviceInterface* pDevice;
	graphics::CreateDeviceInterface(&pDevice);

	graphics::BufferRegion region;
	{
		int result =
			pDevice->CreateVertexBufferRegion(3, t, NULL, uv, &region);

		EXPECT_TRUE(result);
	}
	graphics::DeleteDeviceInterface(pDevice);
}

TEST(TestingDeviceInterface, CreateDynamicBufferRegion)
{
	graphics::DeviceInterface* pDevice;
	graphics::CreateDeviceInterface(&pDevice);

	graphics::BufferRegion region;
	{
		int result =
			pDevice->CreateDynamicBufferRegion(56, NULL, &region);

		EXPECT_TRUE(result);
	}
	graphics::DeleteDeviceInterface(pDevice);
}

TEST(TestingDeviceInterface, CreateStaticBufferRegion)
{
	graphics::DeviceInterface* pDevice;
	graphics::CreateDeviceInterface(&pDevice);

	graphics::BufferRegion region;
	{
		int result =
			pDevice->CreateStaticBufferRegion(56, NULL, &region);

		EXPECT_TRUE(result);
	}
	graphics::DeleteDeviceInterface(pDevice);
}

TEST(TestingDeviceInterface, CreateIndexBufferRegion)
{
	graphics::DeviceInterface* pDevice;
	graphics::CreateDeviceInterface(&pDevice);

	graphics::BufferRegion region;
	{
		int result =
			pDevice->CreateIndexBufferRegion(56, NULL, &region);

		EXPECT_TRUE(result);
	}
	graphics::DeleteDeviceInterface(pDevice);
}

TEST(TestingGraphicsEngine, InitializeAndRunFrame)
{
	using namespace DirectX;
	using namespace graphics;

	UINT
		clientWidth = 1280,
		clientHeight = 720;

	DeviceInterface* pDevice = NULL;
	GraphicsPipeline* pPipeline = NULL;

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

	pDevice->CreateVertexBufferRegion(3, t, NULL, uv, &meshes[0]);
	pDevice->CreateVertexBufferRegion(6, q, NULL, NULL, &meshes[1]);

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

	wnd.Update();
		
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

	UINT at = buffer0.DataLocation;
	UINT end = buffer0.DataLocation + buffer0.DataCount;

	BufferRegion r = buffer0;
	r.DataCount = RenderContext::CB_MAX_BYTES_PER_BIND;

	UINT totalDrawn = 0;
	while (at < end)
	{
		r.DataLocation = at;							// Set active region (65536 bytes per draw)
		pContext->VSSetConstantBuffer(0, r);			// Apply active Region
		pContext->DrawInstanced(1024, 0, meshes[0]);	// Draw (with mesh[1]

		at += RenderContext::CB_MAX_BYTES_PER_BIND;		// Increment active region
	}

	// Present
	wnd.Present();

	pDevice->DeleteRenderTarget(backBuffer);
	pDevice->DeleteDepthBuffer(depthBuffer);
	pDevice->DeleteGraphicsPipeline(pPipeline);
	pDevice->DeletePresentWindow(wnd);

	graphics::DeleteDeviceInterface(pDevice);
}

TEST(TestingDeviceInterface, CreatingAndDeletingGraphicsPipelines)
{
	int result;

	graphics::DeviceInterface* pDevice;
	graphics::CreateDeviceInterface(&pDevice);

	const UINT maxPipelines = graphics::DeviceInterface::MAXIMUM_GRAPHICS_PIPELINES;

	graphics::GraphicsPipeline* pPipeline[maxPipelines];
	{
		// Create maximum allowed pipelines
		for (unsigned int i = 0; i < maxPipelines; i++)
		{
			result = pDevice->CreateGraphicsPipeline(
				gVertexShader,
				gPixelShader,
				&pPipeline[i]);

			EXPECT_TRUE(result);
		}

		// Delete a pipeline and create a new one at the same index
		// to see if it works correctly
		pDevice->DeleteGraphicsPipeline(pPipeline[5]);

		result = pDevice->CreateGraphicsPipeline(
			gVertexShader,
			gPixelShader,
			&pPipeline[5]);

		EXPECT_TRUE(result);

		// Create one extra to see if it fails
		graphics::GraphicsPipeline* pExtraPipeline = NULL;
		result = pDevice->CreateGraphicsPipeline(
			gVertexShader,
			gPixelShader,
			&pExtraPipeline);

		EXPECT_FALSE(result);


		for (unsigned int i = 0; i < maxPipelines; i++)
		{
			pDevice->DeleteGraphicsPipeline(pPipeline[i]);
		}
	}
	graphics::DeleteDeviceInterface(pDevice);
}

TEST(TestingDeviceInterface, CorrectConstantBufferRegionSizeAndLocation)
{
	graphics::DeviceInterface* pDevice;
	graphics::CreateDeviceInterface(&pDevice);

	graphics::BufferRegion regions[10];
	{
		// Creating constant buffer region
		// has alignment of 256 bytes (56 becomes 256)
		for (unsigned int i = 0; i < 10; i++)
		{
			int result =
				pDevice->CreateDynamicBufferRegion(56, NULL, &regions[i]);

			EXPECT_TRUE(result);
		}

		// check alignment is correct
		for (unsigned int i = 0; i < 10; i++)
		{
			EXPECT_EQ(regions[i].DataCount, 256);
			EXPECT_EQ(regions[i].DataLocation, i * 256);
		}
	}
	graphics::DeleteDeviceInterface(pDevice);
}

TEST(TestingDeviceInterface, CorrectBufferRegionSizeAndLocation)
{
	graphics::DeviceInterface* pDevice;
	graphics::CreateDeviceInterface(&pDevice);

	graphics::BufferRegion regions[10];
	{
		// Creating non-costant buffer region
		// will have no alignment
		for (unsigned int i = 0; i < 10; i++)
		{
			int result =
				pDevice->CreateIndexBufferRegion(56, NULL, &regions[i]);

			EXPECT_TRUE(result);
		}

		// check that they are correct size and on correct location
		for (unsigned int i = 0; i < 10; i++)
		{
			EXPECT_EQ(regions[i].DataCount, 56);
			EXPECT_EQ(regions[i].DataLocation, i * 56);
		}
	}
	graphics::DeleteDeviceInterface(pDevice);
}

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	::testing::InitGoogleTest(&argc, argv);
	int testsFailed = RUN_ALL_TESTS();

	system("pause");

	return 0;
}