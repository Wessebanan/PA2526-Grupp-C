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

	SUN_DESC sunDesc = {};
	sunDesc.Red = 200;
	sunDesc.Green = 200;
	sunDesc.Blue = 200;
	
	sunDesc.Direction = { -0.2f, 0.0f, 1.0f };
	sunDesc.Position = { 35.0f, 0.0f, -2.0f };
	
	sunDesc.FarPlane = 100.0f;
	sunDesc.NearPlane = 0.1f;

	sunDesc.Height	= 100.0f;
	sunDesc.Width	= 100.0f;

	sunDesc.Resolution = 1024;

	RenderManager* pMng = new RenderManager;
	pMng->Initialize(sunDesc, 1600, 900, "D3D11");

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

		float3 normals[3] = {
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
		};

		VERTEX_BUFFER_DATA data = { NULL };
		data.VertexCount	= 3;
		data.pVertexData	= triangle;
		data.pNormalsData	= normals;

		mesh_index0 = pMng->CreateMesh(&data, nullptr);
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

		float3 normals[6] = {
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,

			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
		};

		VERTEX_BUFFER_DATA data = { NULL };
		data.VertexCount = 6;
		data.pVertexData = quad;
		data.pNormalsData = normals;

		mesh_index1 = pMng->CreateMesh(&data, nullptr);
	}

	int mesh_index2; // Big Format Quad
	{
		float3 quad[6] = {
		 -20.0f,  -20.0f,	0.0f,
		 -20.0f,   20.0f,	0.0f,
		  20.0f,  -20.0f,	0.0f,

		  20.0f,  -20.0f,	0.0f,
		 -20.0f,   20.0f,	0.0f,
		  20.0f,   20.0f,	0.0f,
		};

		float3 normals[6] = {
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,

			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
		};

		VERTEX_BUFFER_DATA data = { NULL };
		data.VertexCount = 6;
		data.pVertexData = quad;
		data.pNormalsData = normals;

		mesh_index2 = pMng->CreateMesh(&data, nullptr);
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
	MODEL_LAYOUT_DESC m_desc[3];
	m_desc[0].InstanceCount = count / 2;
	m_desc[0].MeshIndex = mesh_index0;

	m_desc[1].InstanceCount = count / 2;
	m_desc[1].MeshIndex = mesh_index1;

	m_desc[2].InstanceCount = 1;
	m_desc[2].MeshIndex = mesh_index2;

	desc[RENDER_DEFAULT].PerInstanceByteWidth = sizeof(float4);
	desc[RENDER_DEFAULT].pModelLayout = m_desc;
	desc[RENDER_DEFAULT].ModelLayoutCount = ARRAYSIZE(m_desc);


	pMng->CreateModelHeap(desc);

	graphics::PresentWindow* pWnd = pMng->GetPresentWindow();

	Default* tri_array = (Default*)pMng->GetTechniqueModelBuffer(RENDER_DEFAULT);

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

	tri_array[count].x		= 20.0f;
	tri_array[count].y		= 20.0f;
	tri_array[count].z		= 10.0f;
	tri_array[count].color	= PACK(255, 255, 255, 0);

	float x		= 10.0f;
	float y		= 0.0f;
	float z		= 1.0f;
	XMFLOAT4X4 viewMatrix;
	TransformViewMatrix(viewMatrix, x, y, z);

	pMng->SetViewMatrix(viewMatrix);

	pWnd->Show();
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			pMng->Clear(0.1f, 0.1f, 0.1f);

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
			pMng->SetViewMatrix(viewMatrix);

			for (UINT x = 0; x < width; x++)
			{
				for (UINT y = 0; y < height; y++)
				{
					UINT index = x * height + y;
					tri_array[index].y -= 0.001f;

					if (tri_array[index].y < 0.0f)
					{
						tri_array[index].y = 10.0f;
					}
				}
			}

			pMng->Draw();
			pWnd->Present();
		}
	}

	pMng->Destroy();
	delete pMng;

	return 0;
}