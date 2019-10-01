#pragma once

#include <string>
#include <DirectXMath.h>

#include "..//includes/rendering/RenderManager.h"

#ifdef _DEBUG
#pragma comment(lib, "RenderEngine_d.lib")
#else
#pragma comment(lib, "RenderEngine.lib")
#endif // DEBUG

TEST(TestingRenderer, CreatingDefaultMesh)
{
	// create vertex data
	struct float3
	{
		float x, y, z;
	};

	float3 vertex_data[3] = {
		-1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
	};

	rendering::RenderManager renderer;
	renderer.Initialize(800, 600, "Testing renderer");

	// assign vertex data
	rendering::VERTEX_BUFFER_DATA vb = { 0 };
	vb.VertexCount = 3;
	vb.pVertexData = vertex_data;

	// create mesh with specified data
	int meshIndex = renderer.CreateMesh(&vb, NULL);
	
	// check return values
	EXPECT_GE(meshIndex, 0);

	renderer.Destroy();
}

TEST(TestingRenderer, CreatingIndexedMesh)
{
	// Create vertex and index data
	struct float3
	{
		float x, y, z;
	};

	float3 vertex_data[4] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	int indices[6] = {
		0, 1, 2,
		1, 2, 3
	};

	rendering::RenderManager renderer;
	renderer.Initialize(800, 600, "Testing renderer");

	// Assign vertex data
	rendering::VERTEX_BUFFER_DATA vb = { 0 };
	vb.VertexCount = 4;
	vb.pVertexData = vertex_data;

	// Assign index data
	rendering::INDEX_BUFFER_DATA ib;
	ib.IndexCount = 6;
	ib.pIndexData = indices;

	// Create mesh with specified data
	int meshIndex = renderer.CreateMesh(&vb, &ib);

	// check return values
	EXPECT_GE(meshIndex, 0);

	renderer.Destroy();
}

TEST(TestingRenderer, LoopThroughAllocatedData)
{
	using namespace rendering;

	RenderManager mng;
	mng.Initialize(1280, 720, "D3D11");

	/* Create Mesh From Previous Tests */

	struct float3
	{
		float x, y, z;
	};

	int meshIndex0;
	{
		float3 triangle[3] = {
			-0.1f,  -0.1f, 0.0f,
			 0.0f,   0.1f, 0.0f,
			 0.1f,  -0.1f, 0.0f,
		};

		VERTEX_BUFFER_DATA data = { NULL };
		data.VertexCount = 3;
		data.pVertexData = triangle;

		meshIndex0 = mng.CreateMesh(&data, nullptr);
	}

	int meshIndex1;
	{
		float3 quad[6] = {
		 -0.1f,  -0.1f, 0.0f,
		 -0.1f,   0.1f, 0.0f,
		  0.1f,  -0.1f, 0.0f,

		  0.1f,  -0.1f, 0.0f,
		 -0.1f,   0.1f, 0.0f,
		  0.1f,   0.1f, 0.0f,
		};

		VERTEX_BUFFER_DATA data = { NULL };
		data.VertexCount = 6;
		data.pVertexData = quad;

		meshIndex1 = mng.CreateMesh(&data, nullptr);
	}

	/* END */

	struct float4
	{
		float x, y, z, w;
	};

	UINT width = 150;
	UINT height = 109;
	UINT count = width * height;

	TECHNIQUE_HEAP_LAYOUT_DESC desc[RENDER_TECHNIQUES_COUNT] = { 0 };

	MODEL_LAYOUT_DESC m_desc;
	m_desc.InstanceCount = count;
	m_desc.MeshIndex = meshIndex0;

	desc[RENDER_DEFAULT].PerInstanceByteWidth = sizeof(float4);
	desc[RENDER_DEFAULT].pModelLayout = &m_desc;
	desc[RENDER_DEFAULT].ModelLayoutCount = 1;

	MODEL_LAYOUT_DESC m_desc0;
	m_desc0.InstanceCount = 1;
	m_desc0.MeshIndex = meshIndex0;

	desc[RENDER_SCREEN_SPACE].PerInstanceByteWidth = sizeof(float4);
	desc[RENDER_SCREEN_SPACE].pModelLayout = &m_desc0;
	desc[RENDER_SCREEN_SPACE].ModelLayoutCount = 1;

	mng.CreateModelHeap(desc);

	float4* triArray = (float4*)mng.GetTechniqueModelBuffer(RENDER_DEFAULT);

	for (UINT x = 0; x < width; x++)
	{
		for (UINT y = 0; y < height; y++)
		{
			UINT index = x * height + y;
			triArray[index].x = x * 0.5f - 0.99f;
			triArray[index].y = y * 0.5f - 0.99f;
		}
	}

	mng.Destroy();
}
