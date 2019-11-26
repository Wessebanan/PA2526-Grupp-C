
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
#include "gameGraphics/InitParticles.h"

#include "gameWorld/InitWorld.h"

#include <time.h>

#include "gameUtility/Timer.h"
#include "gameGameLoop/InitGameLoop.h"
#include "gameGameLoop/GameLoopEvents.h"

#include "InitHttpServer.h"

#include "gameTraps/InitTraps.h"
#include "gameWeapons/InitWeapons.h"
#include "gameTraps/TrapComponents.h"
#include "gameTraps/TrapEvents.h"
#include "gameWorld/InitWorldScenery.h"

#include "gameWeapons/WeaponEvents.h"

#include "gamePowerups/InitPowerups.h"
#include "gamePowerups/PowerupEvents.h"

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

	constexpr UINT RESERVED_COMPONENTS = 50000;
	ecs.reserveComponentCount<ecs::components::TransformComponent>(RESERVED_COMPONENTS);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(RESERVED_COMPONENTS);
	ecs.reserveComponentCount<ecs::components::TileComponent>(RESERVED_COMPONENTS);
	ecs.reserveComponentCount<ecs::components::OceanTileComponent>(RESERVED_COMPONENTS);
	ecs.reserveComponentCount<ecs::components::TrapComponent>(RESERVED_COMPONENTS);
	ecs.reserveComponentCount<ecs::components::ObjectCollisionComponent>(RESERVED_COMPONENTS);

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
			
			// Start a devcamera if pressing E
			if (GetAsyncKeyState('E'))
			{
				ecs.removeSystem<ecs::systems::UpdateDynamicCameraSystem>();
				ecs.createSystem<ecs::systems::UpdateCameraSystem>(0);
			}
			// Start a dyncamera if pressing F
			if (GetAsyncKeyState('F'))
			{
				ecs.removeSystem<ecs::systems::UpdateCameraSystem>();
				ecs.createSystem<ecs::systems::UpdateDynamicCameraSystem>(0);
			}

			if (GetAsyncKeyState(VK_SPACE) && start_once)
			{
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
	
	mesh_manager.Destroy();
	render_manager.Destroy();

	graphics::DestroyD3D11();

	MeshContainer::Terminate();
	render_buffer.Terminate();
	

	return 0;

}
void DebuggFunctions(EntityComponentSystem& rECS)
{
	// FOR DEBBUGING TRAPS: SPAWNS TRAPS OVER THE WHOLE MAP
	//events::PlaceTrapEvent place_event;
	//place_event.type = GAME_OBJECT_TYPE_TRAP_SPRING;
	//components::TileComponent* p_tile;
	//TypeFilter tile_filter;
	//tile_filter.addRequirement(components::TileComponent::typeID);
	//EntityIterator tiles = rECS.getEntititesByFilter(tile_filter);
	//for (FilteredEntity tile : tiles.entities)
	//{
	//	p_tile = tile.getComponent<components::TileComponent>();
	//	if (p_tile->tileType != WATER)
	//	{
	//		place_event.tileID = p_tile->getEntityID();
	//		rECS.createEvent(place_event);
	//	}
	//}
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
	InitParticles(rECS);

	InitAI(rECS);

	InitSound(rECS);
	InitSong(rECS);

	InitInput(rECS);
	InitInterpreter(rECS);

	InitPhysics(rECS);
	InitGrid(rECS);
	InitArmy(rECS);
	InitSceneObjects(rECS);

	CreateCollisionForTiles(rECS);
	CreateCollisionForSceneObjects(rECS);

	InitOceanEntities(rECS);
	InitOceanUpdateSystem(rECS);

	InitCamera(rECS);

	InitAnimation(rECS);

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

	InitWeapons(rECS);

	InitTraps(rECS);
	InitPowerups(rECS);

	InitWorldScenery(rECS);

	InitHttpServer(rECS);


	ecs::events::GameStartEvent eve;
	rECS.createEvent(eve);

	DebuggFunctions(rECS);

	//events::SpawnWeaponEvent spawn_event;
	//spawn_event.weaponType = GAME_OBJECT_TYPE_WEAPON_BOMB;
	//TypeFilter tile_filter;
	//tile_filter.addRequirement(ecs::components::TileComponent::typeID);
	//EntityIterator tiles = rECS.getEntititesByFilter(tile_filter);
	//for (FilteredEntity tile : tiles.entities)
	//{
	//	components::TileComponent* p_tile = tile.getComponent<components::TileComponent>();
	//	if (p_tile->tileType != WATER)
	//	{
	//		spawn_event.spawnTileId = p_tile->getEntityID();
	//		rECS.createEvent(spawn_event);
	//	}
	//}
}