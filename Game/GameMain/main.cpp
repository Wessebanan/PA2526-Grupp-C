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

#include "Physics/InitPhysics.h"

#include <time.h>
#include <chrono>

using namespace ecs;								  

int main()
{

	srand(time(0));

	EntityComponentSystem ecs;

	// Tiles + sceneobjects + units + camera
	ecs.reserveComponentCount<ecs::components::TransformComponent>(144 + 12 + 12 + 1);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(144 + 12 + 12);
	ecs.reserveComponentCount<ecs::components::TileComponent>(144);

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

	ModelLoader::Mesh **pp_meshes = InitMesh(ecs, pMng);
	
	InitPhysics(ecs, pp_meshes);

	graphics::PresentWindow* pWnd = pMng->GetPresentWindow();

	PlaceMesh(ecs, pMng);


	// to get components in the loop
	ecs::ComponentIterator itt;

	itt = ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID);
	ecs::components::CameraComponent* p_cam_comp = (ecs::components::CameraComponent*)itt.next();


	pMng->SetViewMatrix(p_cam_comp->viewMatrix);
	pWnd->Show();
	ecs.update(0.1f);

	// Timer
	std::chrono::system_clock::time_point frame_start = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point frame_end = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsed;
	while (pWnd->IsOpen())
	{

		if (!pWnd->Update())
		{
			// Starting frame right after ecs::update, first frame a little fucky.
			frame_start = std::chrono::system_clock::now();

			pMng->Clear(0.2f, 0.1f, 0.1f);
			
			pMng->SetViewMatrix(p_cam_comp->viewMatrix);
			
			pMng->Draw();
			pWnd->Present();

			// Finding how long passed between each ecs::update.
			frame_end = std::chrono::system_clock::now();
			elapsed = frame_end - frame_start;

			// Passing elapsed time in seconds to ecs.
			ecs.update(elapsed.count());
			
		}
	}
	
	for (int i = 0; i < Mesh::N_MESHES; i++) delete pp_meshes[i];
	delete[] pp_meshes;

	pMng->Destroy();
	return 0;
}