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

	rendering::SUN_DESC sunDesc;

	rendering::RenderManager *renderer = new rendering::RenderManager;
	renderer->Initialize(sunDesc, 800, 600, "Testing renderer");

	// assign vertex data
	rendering::VERTEX_BUFFER_DATA vb = { 0 };
	vb.VertexCount = 3;
	vb.pVertexData = vertex_data;

	// create mesh with specified data
	int meshIndex = renderer->CreateMesh(&vb, NULL);
	
	// check return values
	EXPECT_GE(meshIndex, 0);

	renderer->Destroy();
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

	rendering::SUN_DESC sunDesc;
	rendering::RenderManager* renderer = new rendering::RenderManager;
	renderer->Initialize(sunDesc, 800, 600, "Testing renderer");

	// Assign vertex data
	rendering::VERTEX_BUFFER_DATA vb = { 0 };
	vb.VertexCount = 4;
	vb.pVertexData = vertex_data;

	// Assign index data
	rendering::INDEX_BUFFER_DATA ib;
	ib.IndexCount = 6;
	ib.pIndexData = indices;

	// Create mesh with specified data
	int meshIndex = renderer->CreateMesh(&vb, &ib);

	// check return values
	EXPECT_GE(meshIndex, 0);

	renderer->Destroy();
}

TEST(TestingRenderer, LoopThroughAllocatedDataAndDrawAFrame)
{
	using namespace rendering;
	using namespace DirectX;

	rendering::SUN_DESC sunDesc;
	RenderManager* mng = new RenderManager;
	mng->Initialize(sunDesc, 1280, 720, "D3D11");

	struct float3
	{
		float x, y, z;
	};

	int mesh_index0; // Triangle
	{
		float3 triangle[3] = {
			-0.1f,  -0.1f, 0.0f,
			 0.0f,   0.1f, 0.0f,
			 0.1f,  -0.1f, 0.0f,
		};

		VERTEX_BUFFER_DATA data = { NULL };
		data.VertexCount = 3;
		data.pVertexData = triangle;

		mesh_index0 = mng->CreateMesh(&data, nullptr);
	}

	int mesh_index1; // Quad
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

		mesh_index1 = mng->CreateMesh(&data, nullptr);
	}

	struct float4
	{
		float x, y, z, w;
	};

	UINT width = 2;
	UINT height = 2;
	UINT count = width * height;

	TECHNIQUE_HEAP_LAYOUT_DESC desc[RENDER_TECHNIQUES_COUNT] = { 0 };

	// Default Technique will render 'count' meshes in white
	MODEL_LAYOUT_DESC m_desc[2];
	m_desc[0].InstanceCount = count / 2;
	m_desc[0].MeshIndex = mesh_index0;

	m_desc[1].InstanceCount = count / 2;
	m_desc[1].MeshIndex = mesh_index1;

	desc[RENDER_DEFAULT].PerInstanceByteWidth = sizeof(float4);
	desc[RENDER_DEFAULT].pModelLayout = m_desc;
	desc[RENDER_DEFAULT].ModelLayoutCount = ARRAYSIZE(m_desc);


	mng->CreateModelHeap(desc);

	graphics::PresentWindow* pWnd = mng->GetPresentWindow();

	float4* tri_array = (float4*)mng->GetTechniqueModelBuffer(RENDER_DEFAULT);

	for (UINT x = 0; x < width; x++)
	{
		for (UINT y = 0; y < height; y++)
		{
			UINT index = x * height + y;
			tri_array[index].x = x * 0.5f - 0.99f;
			tri_array[index].y = y * 0.5f - 0.99f;

			tri_array[index].w = 1.0f;
		}
	}

	float x = 2.0f;
	float z = 1.0f;
	XMFLOAT4X4 viewMatrix;
	XMStoreFloat4x4(&viewMatrix,
		XMMatrixLookToLH(
			{ x, 2.0f, z },
			{ 0.0f, 0.0f,  1.0f },
			{ 0.0f, 1.0f,  0.0f }
	));

	mng->SetViewMatrix(viewMatrix);

	mng->Clear(0.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&viewMatrix,
		XMMatrixLookToLH(
			{ x, 0.0f, z },
			{ 0.0f, 0.0f,  1.0f },
			{ 0.0f, 1.0f,  0.0f }
	));

	mng->SetViewMatrix(viewMatrix);

	mng->Draw();
	pWnd->Present();

	mng->Destroy();
}
