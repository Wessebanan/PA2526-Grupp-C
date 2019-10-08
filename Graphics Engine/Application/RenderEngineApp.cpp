#include "..//includes/rendering/RenderManager.h"

#ifdef _DEBUG
#pragma comment(lib, "RenderEngine_d.lib")
#else
#pragma comment(lib, "RenderEngine.lib")
#endif // DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

void TransformViewMatrix(
	DirectX::XMFLOAT4X4& rViewMatrix, 
	const float x, 
	const float y, 
	const float z)
{
	DirectX::XMStoreFloat4x4(&rViewMatrix,
		DirectX::XMMatrixLookToLH(
			{ x, y, z },
			{ 0.0f, 0.0f,  1.0f },
			{ 0.0f, 1.0f,  0.0f }
	));
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	using namespace rendering;
	using namespace DirectX;

	RenderManager mng;
	mng.Initialize(1600, 900, "D3D11");

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

		mesh_index0 = mng.CreateMesh(&data, nullptr);
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

		mesh_index1 = mng.CreateMesh(&data, nullptr);
	}

	struct float4
	{
		float x, y, z, w;
	};

	struct Default
	{
		float x, y, z;
		uint32_t color;
	};

	UINT width = 160;
	UINT height = 100;
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

	// SCREEN_SPACE will render 2 blue meshes (quad and triangle)
	MODEL_LAYOUT_DESC m_desc0[2];
	m_desc0[0].InstanceCount	= 1;
	m_desc0[0].MeshIndex		= mesh_index1;

	m_desc0[1].InstanceCount	= 1;
	m_desc0[1].MeshIndex		= mesh_index0;

	desc[RENDER_SCREEN_SPACE].PerInstanceByteWidth		= sizeof(float4);
	desc[RENDER_SCREEN_SPACE].pModelLayout				= m_desc0;
	desc[RENDER_SCREEN_SPACE].ModelLayoutCount			= 2;


	mng.CreateModelHeap(desc);

	graphics::PresentWindow* pWnd = mng.GetPresentWindow();

	Default* tri_array = (Default*)mng.GetTechniqueModelBuffer(RENDER_DEFAULT);

	for (UINT x = 0; x < width; x++)
	{
		for (UINT y = 0; y < height; y++)
		{
			UINT index = x * height + y;
			tri_array[index].x = x * 0.5f - 0.99f;
			tri_array[index].y = y * 0.5f - 0.99f;

			tri_array[index].color = PACK(5, 50, 240, 0);
		}
	}

	float4* water = (float4*)mng.GetTechniqueModelBuffer(RENDER_SCREEN_SPACE);
	water->x = -0.10f;
	water->y =  0.70f;

	float x = 10.0f;
	float y = 23.0f;
	float z = 1.0f;
	XMFLOAT4X4 viewMatrix;
	TransformViewMatrix(viewMatrix, x, y, z);

	mng.SetViewMatrix(viewMatrix);

	pWnd->Show();
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			mng.Clear(0.1f, 0.1f, 0.1f);

			float moveSpeed = 0.01f;
			if (GetAsyncKeyState(VK_UP))
			{
				z += moveSpeed;
			}

			if (GetAsyncKeyState(VK_DOWN))
			{
				z -= moveSpeed;
			}

			if (GetAsyncKeyState(VK_LEFT))
			{
				x -= moveSpeed;
			}

			if (GetAsyncKeyState(VK_RIGHT))
			{
				x += moveSpeed;
			}

			TransformViewMatrix(viewMatrix, x, y, z);

			mng.SetViewMatrix(viewMatrix);

			mng.Draw();
			pWnd->Present();
		}
	}

	mng.Destroy();

	return 0;
}