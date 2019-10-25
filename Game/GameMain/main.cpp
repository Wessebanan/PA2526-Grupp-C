
#include "Input/InitInput.h"
#include "../../Graphics/includes/Window.h"
#include "ecs.h"



//#include "gameRendering/InitMesh.h"
//#include "gameRendering/PlaceMesh.h"

#include "gameAI/InitArmy.h"
#include "gameAI/InitGrid.h"
#include "gameAI/InitAI.h"
#include "AIGlobals.h"

#include "Input/InitInterpreter.h"
#include "UI/InitUI.h"
#include "Direct2D.h"

#include "gameUtility/InitCamera.h"

#include "gameSceneObjects/InitSceneObjectsh.h"
#include "gameSceneObjects/InitBiomes.h"
#include "gameUtility/UtilityEcsFunctions.h"

#include "gameAudio/InitAudio.h"

#include "Physics/InitPhysics.h"

#include "Mesh.h"


#include "../../Graphics/includes/RenderManager.h"
#include "../../Graphics/includes/MeshManager.h"

#include "gameAnimation/InitAnimation.h"

#include "Renderers/Renderers.h"

#include "gameGraphics/GraphicsECSSystems.h"
#include "gameGraphics/InitGraphics.h"

#include "gameWorld/InitWorld.h"

#include <time.h>

#include "gameUtility/Timer.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

void InitAll(EntityComponentSystem& rECS, HWND window);

const UINT g_RENDER_BUFFER_SIZE = PAD(pow(10, 6), 256);


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	
	srand(time(0));

	/*
		-- Client Resolution --
	*/
	const UINT
		client_width = graphics::GetDisplayResolution().x,
		client_height = graphics::GetDisplayResolution().y;

	graphics::Window wnd;
	wnd.Initialize(
		client_width,
		client_height,
		"Couch Commanders",
		graphics::WINDOW_STYLE::BORDERLESS);

	/*
		-- Initialize Graphics --
	*/

	graphics::InitializeD3D11();
	graphics::AttachHwndToSwapChain(wnd);

	/*
		-- Initialize ECS --
	*/

	ecs::EntityComponentSystem ecs;
	TempUISystemPtrs my_UI_systems;

	ecs.reserveComponentCount<ecs::components::TransformComponent>(5000);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(5000);
	ecs.reserveComponentCount<ecs::components::TileComponent>(5000);
	ecs.reserveComponentCount<ecs::components::OceanTileComponent>(5000);

	/*
		InitAll is a list of ecs system Init-functions.
		If you with to add your own Init-function, please add in
		in InitAll().
	*/

	InitAll(ecs, HWND(wnd));

	/*
		 #############################                                                    ############################# 
		#######################   From here on, all initialization is expected to be finished.   #######################
		 #############################                                                    ############################# 
	*/


	/*
		-- Show Window --
	*/

	wnd.Open();

	/*
		-- Create Timer for update loop --
	*/

	Timer timer;
	timer.StartGame();

	/*
		-- Update Loop, while window is open --
	*/

	while (wnd.IsOpen())
	{
		if (!wnd.Update())
		{ 
			// Close window when user press Esc
			if (GetAsyncKeyState(VK_ESCAPE))
			{
				wnd.Close();
			}

			/*
				Update all ECS systems, and give them the delta time.
			*/
			ecs.update(timer.GetFrameTime());

			graphics::Present(0);
		}
	}

	/*
		-- Cleanup Memory --
	*/

	graphics::RenderManager& render_manager = static_cast<components::RenderManagerComponent*>(ecs.getAllComponentsOfType(components::RenderManagerComponent::typeID).next())->mgr;
	graphics::MeshManager& mesh_manager = static_cast<components::MeshManagerComponent*>(ecs.getAllComponentsOfType(components::MeshManagerComponent::typeID).next())->mgr;
	graphics::RenderBuffer& render_buffer = static_cast<components::RenderBufferComponent*>(ecs.getAllComponentsOfType(components::RenderBufferComponent::typeID).next())->buffer;

	mesh_manager.Destroy();
	render_manager.Destroy();

	graphics::DestroyD3D11();

	MeshContainer::Terminate();
	render_buffer.Terminate();

	return 0;

}

void InitAll(EntityComponentSystem& rECS, HWND window)
{
	/*
		List all Init functions that will create ECS systems.
		Remember to syncronize layers between systems. A system
		will only recieve an event if the system that creates
		the events exist in a layer previous to the listener;
		as all events are cleared at the end of each ecs update,
		after all ecs systems has been updated.
	*/
	
	TempUISystemPtrs ui_systems;
	InitGraphicsComponents(rECS, g_RENDER_BUFFER_SIZE, graphics::GetDisplayResolution().x, graphics::GetDisplayResolution().y);
	InitMeshes(rECS);
	InitGraphicsPreRenderSystems(rECS);

	InitSound(rECS);
	InitSong(rECS);

	InitAI(rECS);
	

	InitInput(rECS);
	InitInterpreter(rECS);

	InitGrid(rECS);
	InitArmy(rECS);
	InitSceneObjects(rECS);

	InitOceanEntities(rECS);

	InitCamera(rECS);

	InitPhysics(rECS, MeshContainer::GetMeshCPU(MESH_TYPE_UNIT));

	InitGraphicsRenderSystems(rECS);
	InitGraphicsPostRenderSystems(rECS);

	InitUI(rECS, ui_systems, window);
	ChangeUserStateEvent e;
	e.newState = ATTACK;
	e.playerId = PLAYER1;
	rECS.createEvent(e);
	e.playerId = PLAYER2;
	rECS.createEvent(e);
	e.playerId = PLAYER3;
	rECS.createEvent(e);
	e.playerId = PLAYER4;
	rECS.createEvent(e);
}