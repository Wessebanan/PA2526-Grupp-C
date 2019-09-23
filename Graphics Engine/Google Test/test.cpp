#include "pch.h"
#include "../includes/DeviceInterface.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

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

void RunTests(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int testsFailed = RUN_ALL_TESTS();
}

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	RunTests(argc, argv);

	system("pause");

	return 0;
}