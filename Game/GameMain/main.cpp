//#include "InputBackend.h"
#include "Rendering/InitMeshes.h"

#include "rendering/RenderManager.h"


using namespace ecs;

int main()
{
	//EntityComponentSystem ecs;
	// tiles + units + camera
	//ecs.reserveComponentCount<components::TransformComponent>(144 + 12 + 1);

	//initInput(ecs);

	// Init Render Manger outside to use it elsewhere
	//RenderManager mng;
	//mng.Initialize(1280, 720, "Lhure e best");

	//InitMeshes(ecs, mng);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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



	graphics::PresentWindow* pWnd = mng.GetPresentWindow();

	pWnd->Show();
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			mng.Clear(0.2f, 0.1f, 0.1f);



	//ecs.update(0.0f);
			//mng.Draw();
			pWnd->Present();
		}
	}

	return 0;
}