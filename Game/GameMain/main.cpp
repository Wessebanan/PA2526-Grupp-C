#include "ecs.h"
#include "gameRendering/InitMesh.h"
#include "gameRendering/PlaceMesh.h"

#include "gameAI/InitArmy.h"
#include "gameAI/InitGrid.h"

#include "gameUtility/CameraFunctions.h"

#include "Input/InitInput.h"
#include "Input/InitInterpreter.h"

#include "gameSound/InitSound.h"

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

	// Tiles + units + camera
	ecs.reserveComponentCount<ecs::components::TransformComponent>(144 + 12 + 1);
	ecs.reserveComponentCount<ecs::components::TileComponent>(144);

	InitSound(ecs);
	// TEMPORARY
	// Just to play something to know it works
	ecs::events::PlaySound sound_event;
	sound_event.soundName = SoundName::COIN_TEST_SOUND;
	sound_event.soundFlags = SoundFlags::NONE;
	ecs.createEvent(sound_event);

	InitInput(ecs);
	InitInterpreter(ecs);
	//CameraFunctions::CreateDevCamera(ecs);


	InitGrid(ecs);
	InitArmy(ecs);

	rendering::SUN_DESC sun_desc;
	sun_desc.Red = 200;
	sun_desc.Green = 200;
	sun_desc.Blue = 200;
	
	sun_desc.Direction	= { 0.8f, -1.0f, 0.0f };
	sun_desc.Position	= { -4.0f, 8.0f, 10.0f };

	sun_desc.NearPlane = 0.1f;
	sun_desc.FarPlane = 100.0f;

	sun_desc.Width = 25.0f;
	sun_desc.Height = 25.0f;

	sun_desc.Resolution = 2048;
	
	rendering::RenderManager* mng = new rendering::RenderManager;
	mng->Initialize(sun_desc, 1600, 900, "D3D11");


	InitMesh(mng);

	graphics::PresentWindow* pWnd = mng->GetPresentWindow();

	PlaceMesh(ecs, mng);

	float x = 6.0f;
	float y = 5.0f;
	float z = -10.0f;
	XMFLOAT4X4 view_matrix;
	
	TransformViewMatrix(view_matrix, x, y, z);

	mng->SetViewMatrix(view_matrix);


	// to get components in the loop
	ecs::ComponentIterator itt;
	pWnd->Show();
	ecs.update(0.1f);
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			mng->Clear(0.2f, 0.1f, 0.1f);
			

			// Moves the camera wiht input, should be removed when camera gets implemented
			itt = ecs.getAllComponentsOfType(ecs::components::KeyboardComponent::typeID);
			ecs::components::KeyboardComponent* p_kb = (ecs::components::KeyboardComponent*)itt.next();

			float move_speed = 0.01f;
			if (p_kb->W)
			{
				z += move_speed;
			}

			if (p_kb->S)
			{
				z -= move_speed;
			}

			if (p_kb->A)
			{
				x -= move_speed;
			}

			if (p_kb->D)
			{
				x += move_speed;
			}

			TransformViewMatrix(view_matrix, x, y, z);

			mng->SetViewMatrix(view_matrix);
			
			mng->Draw();
			pWnd->Present();

			ecs.update(0.1f);
		}
	}

	mng->Destroy();
	return 0;
}