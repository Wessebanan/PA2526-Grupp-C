#include "..//includes/rendering/RenderManager.h"

#ifdef _DEBUG
#pragma comment(lib, "RenderEngine_d.lib")
#else
#pragma comment(lib, "RenderEngine.lib")
#endif // DEBUG

int main()
{
	using namespace rendering;

	RenderManager mng;
	mng.Initialize();

	struct float3
	{
		float x, y, z;
	};

	int meshIndex0;
	{
		float3 triangle[3] = {
			-0.01f,  -0.01f, 0.0f,
			  0.0f,   0.01f, 0.0f,
			 0.01f,  -0.01f, 0.0f,
		};

		VERTEX_BUFFER_DATA data = { NULL };
		data.VertexCount = 3;
		data.pVertexData = triangle;

		meshIndex0 = mng.CreateMesh(&data, nullptr);
	}

	int meshIndex1;
	{
		float3 quad[6] = {
		 -0.01f,  -0.01f, 0.0f,
		 -0.01f,   0.01f, 0.0f,
		  0.01f,  -0.01f, 0.0f,

		  0.01f,  -0.01f, 0.0f,
		 -0.01f,   0.01f, 0.0f,
		  0.01f,   0.01f, 0.0f,
		};

		VERTEX_BUFFER_DATA data = { NULL };
		data.VertexCount = 6;
		data.pVertexData = quad;

		meshIndex1 = mng.CreateMesh(&data, nullptr);
	}

	struct float4
	{
		float x, y, z, w;
	};

	UINT width = 100;
	UINT height = 110;
	UINT count = width * height;

	MODEL_LAYOUT_DESC m_desc[2];
	m_desc[0].InstanceCount = count / 2;
	m_desc[0].MeshIndex = meshIndex0;

	m_desc[1].InstanceCount = count / 2;
	m_desc[1].MeshIndex = meshIndex1;


	MODEL_LAYOUT_DESC m_desc0;
	m_desc0.InstanceCount = 1;
	m_desc0.MeshIndex = meshIndex1;

	TECHNIQUE_HEAP_LAYOUT_DESC desc[RENDER_TECHNIQUES_COUNT] = { 0 };
	desc[RENDER_DEFAULT].PerInstanceByteWidth	= sizeof(float4);
	desc[RENDER_DEFAULT].pModelLayout			= m_desc;
	desc[RENDER_DEFAULT].ModelLayoutCount		= ARRAYSIZE(m_desc);

	desc[RENDER_WATER].PerInstanceByteWidth = sizeof(float4);
	desc[RENDER_WATER].pModelLayout = &m_desc0;
	desc[RENDER_WATER].ModelLayoutCount = 1;

	mng.CreateModelHeap(desc);

	graphics::PresentWindow* pWnd = mng.SetWndSettings(1280, 720, "D3D11");

	float4* triArray = (float4*)mng.GetTechniqueModelBuffer(RENDER_DEFAULT);

	for (UINT x = 0; x < width; x++)
	{
		for (UINT y = 0; y < height; y++)
		{
			UINT index = x * height + y;
			triArray[index].x = x * 0.02f - 0.99f;
			triArray[index].y = y * 0.02f - 0.99f;
		}
	}


	pWnd->Show();
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			mng.Clear();

			for (UINT x = 0; x < width; x++)
			{
				for (UINT y = 0; y < height; y++)
				{
					UINT index = x * height + y;
					triArray[index].y -= 0.001f;

					if (triArray[index].y < -1.1f)
					{
						triArray[index].y = 1.1f;
					}
				}
			}

			mng.Draw();
			pWnd->Present();
		}
	}

	mng.Destroy();

	return 0;
}