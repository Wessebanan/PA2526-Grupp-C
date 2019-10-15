#include "ecs.h"
#include "gameRendering/InitMesh.h"
#include "gameRendering/PlaceMesh.h"

#include "gameAI/InitArmy.h"
#include "gameAI/InitGrid.h"

#include "Input/InitInput.h"
#include "Input/InitInterpreter.h"

#include "gameUtility/InitCamera.h"

#include "gameSceneObjects/InitSceneObjectsh.h"

#include "gameAudio/InitAudio.h"

#include "gameSceneObjects/InitOcean.h"
#include "gameSceneObjects/OceanSettings.h"

#include <time.h>

using namespace ecs;								  

int main()
{

	srand(time(0));

	EntityComponentSystem ecs;

	const unsigned int LOGIC_TILE_COUNT = 144;
	const unsigned int SCENE_OBJECT_COUNT = 12;
	const unsigned int UNIT_COUNT = 12;
	const unsigned int CAMERA_COUNT = 1;

	const unsigned int COMPONENT_TRANSFORM_COUNT = LOGIC_TILE_COUNT + OCEAN_TILE_COUNT + SCENE_OBJECT_COUNT + UNIT_COUNT + CAMERA_COUNT;
	const unsigned int COMPONENT_COLOR_COUNT = LOGIC_TILE_COUNT + OCEAN_TILE_COUNT + SCENE_OBJECT_COUNT + UNIT_COUNT;

	// Tiles + sceneobjects + units + camera
	ecs.reserveComponentCount<ecs::components::TransformComponent>(COMPONENT_TRANSFORM_COUNT);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(COMPONENT_COLOR_COUNT);

	ecs.reserveComponentCount<ecs::components::TileComponent>(LOGIC_TILE_COUNT);
	ecs.reserveComponentCount<ecs::components::OceanTileComponent>(OCEAN_TILE_COUNT);


	InitSound(ecs);

	InitInput(ecs);
	InitInterpreter(ecs);
	//CameraFunctions::CreateDevCamera(ecs);


	InitGrid(ecs);
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
	
	rendering::RenderManager* pMng = new rendering::RenderManager;
	pMng->Initialize(sun_desc, 1600, 900, "D3D11");
	InitCamera(ecs);


	
	InitMesh(ecs, pMng);

	InitOcean(ecs);

	graphics::PresentWindow* pWnd = pMng->GetPresentWindow();

	PlaceMesh(ecs, pMng);


	// to get components in the loop
	ecs::ComponentIterator itt;

	itt = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	ecs::components::CameraComponent* p_cam_comp = (ecs::components::CameraComponent*)itt.next();

	pMng->SetViewMatrix(p_cam_comp->viewMatrix);
	pWnd->Show();
	ecs.update(0.1f);
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			pMng->Clear(0.2f, 0.1f, 0.1f);
			

			pMng->SetViewMatrix(p_cam_comp->viewMatrix);
			
			pMng->Draw();
			pWnd->Present();

			ecs.update(0.1f);
		}
	}

	pMng->Destroy();
	return 0;
}