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
	CameraFunctions::CreateDevCamera(ecs);

	
	rendering::RenderManager mng;
	mng.Initialize(1600, 900, "D3D11");


	InitMesh(mng);

	graphics::PresentWindow* pWnd = mng.GetPresentWindow();

	PlaceMesh(ecs, mng);
	ecs::ComponentIterator itt = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	components::CameraComponent* camComp;
	camComp = (components::CameraComponent*)itt.next();
	mng.SetViewMatrix(camComp->viewMatrix);

	float x = 10.0f;
	float y = 13.0f;
	float z = 1.0f;
	XMFLOAT4X4 viewMatrix;
	
	TransformViewMatrix(viewMatrix, x, y, z);

	mng.SetViewMatrix(viewMatrix);




	pWnd->Show();
	ecs.update(0.1f);
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			mng.Clear(0.2f, 0.1f, 0.1f);
				
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
			//mng.SetViewMatrix(camComp->viewMatrix);
			
			mng.Draw();
			pWnd->Present();

			ecs.update(0.1f);
		}
	}

	mng.Destroy();
	return 0;
}