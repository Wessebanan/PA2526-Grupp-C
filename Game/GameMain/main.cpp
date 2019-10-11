#include "ecs.h"
#include "gameRendering/InitMesh.h"
#include "gameRendering/PlaceMesh.h"

#include "gameAI/InitArmy.h"
#include "gameAI/InitGrid.h"

#include "Input/InitInput.h"
#include "Input/InitInterpreter.h"

#include "gameUtility/InitCamera.h"

#include "gameSceneObjects/InitSceneObjectsh.h"
#include "gameSceneObjects/InitBiomes.h"

#include "gameSound/InitSound.h"


using namespace ecs;

int main()
{
	EntityComponentSystem ecs;

	// Tiles + sceneobjects + units + camera
	ecs.reserveComponentCount<ecs::components::TransformComponent>(144 + 12 + 12 + 1);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(144 + 12 + 12);
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
	InitBiomes(ecs);
	InitArmy(ecs);
	InitSceneObjects(ecs);

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
	InitCamera(ecs);


	
	InitMesh(ecs, mng);

	graphics::PresentWindow* pWnd = mng->GetPresentWindow();

	PlaceMesh(ecs, mng);


	// to get components in the loop
	ecs::ComponentIterator itt;

	itt = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	ecs::components::CameraComponent* p_cam_comp = (ecs::components::CameraComponent*)itt.next();


	mng.SetViewMatrix(p_cam_comp->viewMatrix);
	pWnd->Show();
	ecs.update(0.1f);
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			mng->Clear(0.2f, 0.1f, 0.1f);
			

			mng->SetViewMatrix(p_cam_comp->viewMatrix);
			
			mng->Draw();
			pWnd->Present();

			ecs.update(0.1f);
		}
	}

	mng->Destroy();
	return 0;
}