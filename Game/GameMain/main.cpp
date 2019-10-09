#include "ecs.h"
#include "gameRendering/InitMesh.h"
#include "gameRendering/PlaceMesh.h"
#include "gameAI/InitArmy.h"
#include "gameAI/InitGrid.h"
#include "gameUtility/CameraFunctions.h"


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

using namespace ecs;

int main()
{
	EntityComponentSystem ecs;

	ecs.reserveComponentCount<ecs::components::TransformComponent>(144 + 12 + 1);
	ecs.reserveComponentCount<ecs::components::TileComponent>(144);

	



	InitGrid(ecs);
	InitArmy(ecs);

	
	rendering::RenderManager mng;
	mng.Initialize(1600, 900, "D3D11");


	InitMesh(mng);

	graphics::PresentWindow* pWnd = mng.GetPresentWindow();

	PlaceMesh(ecs, mng);

	float x = 6.0f;
	float y = 5.0f;
	float z = -10.0f;
	XMFLOAT4X4 view_matrix;
	
	TransformViewMatrix(view_matrix, x, y, z);

	mng.SetViewMatrix(view_matrix);




	pWnd->Show();
	ecs.update(0.1f);
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			mng.Clear(0.2f, 0.1f, 0.1f);
				
			float move_speed = 0.01f;
			if (GetAsyncKeyState(VK_UP))
			{
				z += move_speed;
			}

			if (GetAsyncKeyState(VK_DOWN))
			{
				z -= move_speed;
			}

			if (GetAsyncKeyState(VK_LEFT))
			{
				x -= move_speed;
			}

			if (GetAsyncKeyState(VK_RIGHT))
			{
				x += move_speed;
			}

			TransformViewMatrix(view_matrix, x, y, z);

			mng.SetViewMatrix(view_matrix);
			
			mng.Draw();
			pWnd->Present();

			ecs.update(0.1f);
		}
	}

	mng.Destroy();
	return 0;
}