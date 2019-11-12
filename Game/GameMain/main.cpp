
#include "Input/InitInput.h"
#include "../../Graphics/includes/Window.h"
#include "ecs.h"

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
#include "gameSceneObjects/InitLoot.h"

#include "gameUtility/UtilityEcsFunctions.h"

#include "gameAudio/InitAudio.h"

#include "Physics/InitPhysics.h"

#include "Mesh.h"


#include "../../Graphics/includes/RenderManager.h"
#include "../../Graphics/includes/MeshManager.h"

#include "gameGraphics/ForwardRenderingPipeline.h"
#include "gameGraphics/ShadowMapPipeline.h"
#include "gameGraphics/SSAOPipeline.h"
#include "gameGraphics/CombineSSAOPipeline.h"
#include "gameGraphics/BlurPipeline.h"

#include "gameAnimation/InitAnimation.h"

#include "Renderers/Renderers.h"

#include "gameGraphics/GraphicsECSSystems.h"
#include "gameGraphics/InitGraphics.h"

#include "gameWorld/InitWorld.h"

#include <time.h>

#include "gameUtility/Timer.h"

#include "gameGameLoop/InitGameLoop.h"
#include "gameGameLoop/GameLoopEvents.h"

#include "InitHttpServer.h"

//#include "gameTraps/InitTraps.h"
#include "gameTraps/TrapSystems.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

void InitAll(EntityComponentSystem& rECS, const UINT clientWidth, const UINT clientHeight);

const UINT g_RENDER_BUFFER_SIZE = PAD(pow(10, 6), 256);


#define MAPSIZETEST true


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	GridProp* p_gp = GridProp::GetInstance();
	p_gp->mCurrentMap = 1;

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

	InitAll(ecs, client_width, client_height);

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
	bool start_once = true;
	int kill_me = 0;
	while (wnd.IsOpen())
	{
		if (!wnd.Update())
		{ 
			// Close window when user press Esc
			if (GetAsyncKeyState(VK_ESCAPE))
			{
				wnd.Close();
			}

			if (GetAsyncKeyState(VK_SPACE) && start_once)
			{
				{
					ecs::events::PlayMusic m_event;
					m_event.audioName = AudioName::CC_TEST_SONG;
					ecs.createEvent(m_event);
				}
				{
					ecs::events::MusicSetVolume m_event;
					m_event.volume = 0.0f;
					ecs.createEvent(m_event);
				}
				{
					ecs::events::FadeInMusic m_event;
					m_event.fadeInTimeInSeconds = 2.0f;
					ecs.createEvent(m_event);
				}
				{
					ecs::events::TriggerFireTrapEvent m_event;
					ComponentIterator itt;
					itt = ecs.getAllComponentsOfType(ecs::components::UnitComponent::typeID);
					while (UnitComponent* unit = (UnitComponent*)itt.next())
					{
						m_event.unitID = unit->getID();
						ecs.createEvent(m_event);
					}
				}
				
				

				start_once = false;
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
	StopHttpServer();

	graphics::RenderManager& render_manager = static_cast<components::RenderManagerComponent*>(ecs.getAllComponentsOfType(components::RenderManagerComponent::typeID).next())->mgr;
	graphics::MeshManager& mesh_manager = static_cast<components::MeshManagerComponent*>(ecs.getAllComponentsOfType(components::MeshManagerComponent::typeID).next())->mgr;
	graphics::RenderBuffer& render_buffer = static_cast<components::RenderBufferComponent*>(ecs.getAllComponentsOfType(components::RenderBufferComponent::typeID).next())->buffer;

	//renderer_ssao.Destroy();
	mesh_manager.Destroy();
	render_manager.Destroy();

	graphics::DestroyD3D11();

	MeshContainer::Terminate();
	render_buffer.Terminate();
	

	return 0;

}

void InitAll(EntityComponentSystem& rECS, const UINT clientWidth, const UINT clientHeight)
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
	InitGraphicsComponents(rECS, g_RENDER_BUFFER_SIZE, clientWidth, clientHeight);
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
	InitOceanUpdateSystem(rECS);

	InitCamera(rECS);

	InitAnimation(rECS);
	InitPhysics(rECS, MeshContainer::GetMeshCPU(GAME_OBJECT_TYPE_UNIT));

	//initTrapTriggers(rECS);#include "ecs.h"
	rECS.createSystem<ecs::systems::FireTrapEventSystem>(9);
	InitGameLoop(rECS);

	WorldMeshData mapMeshData;
	WorldMeshData oceanMeshData;
	TypeFilter map_filter;
	TypeFilter ocean_filter;

	map_filter.addRequirement(components::TransformComponent::typeID);
	map_filter.addRequirement(components::ColorComponent::typeID);
	map_filter.addRequirement(components::TileComponent::typeID);

	ocean_filter.addRequirement(components::TransformComponent::typeID);
	ocean_filter.addRequirement(components::ColorComponent::typeID);
	ocean_filter.addRequirement(components::OceanTileComponent::typeID);

	GenerateTileMesh(rECS, &mapMeshData.pMesh, mapMeshData.vertexCount, map_filter);
	GenerateTileMesh(rECS, &oceanMeshData.pMesh, oceanMeshData.vertexCount, ocean_filter);

	InitGraphicsRenderSystems(rECS, mapMeshData, oceanMeshData, clientWidth, clientHeight);
	InitGraphicsPostRenderSystems(rECS);
	InitUI(rECS, ui_systems);
	initArmyText(rECS);

	InitSpawnLootSystem(rECS);
	InitHttpServer(rECS);


	ecs::events::GameStartEvent eve;
	rECS.createEvent(eve);

}