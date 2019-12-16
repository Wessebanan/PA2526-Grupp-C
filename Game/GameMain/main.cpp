
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
#include "InitIslands.h"

#include "gameWeapons/WeaponEvents.h"

#include "gamePowerups/InitPowerups.h"
#include "gamePowerups/PowerupEvents.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

void InitAll(EntityComponentSystem& rECS, const UINT clientWidth, const UINT clientHeight);

const UINT g_RENDER_BUFFER_SIZE = PAD(pow(10, 6), 256);


#define MAPSIZETEST true

#include "gameWeapons/WeaponSpawner.h"

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
	constexpr UINT LES_RESERVED_COMPONENTS = 500;
	ecs.reserveComponentCount<ecs::components::TransformComponent>(RESERVED_COMPONENTS);
	ecs.reserveComponentCount<ecs::components::ColorComponent>(RESERVED_COMPONENTS);
	ecs.reserveComponentCount<ecs::components::TileComponent>(RESERVED_COMPONENTS);
	ecs.reserveComponentCount<ecs::components::OceanTileComponent>(RESERVED_COMPONENTS);
	ecs.reserveComponentCount<ecs::components::ObjectCollisionComponent>(RESERVED_COMPONENTS);

	ecs.reserveComponentCount<ecs::components::TrapComponent>(LES_RESERVED_COMPONENTS);
	ecs.reserveComponentCount<ecs::components::SpringRetractionComponent>(LES_RESERVED_COMPONENTS);
	ecs.reserveComponentCount<ecs::components::TrapQueueInfoComponent>(LES_RESERVED_COMPONENTS);

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
		if (!wnd.Update()/* && graphics::HasFocus(wnd)*/)
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
			// RESTART THE GAME WITH O
			if (GetAsyncKeyState('C'))
			{
				events::GameReStartEvent eve;
				ecs.createEvent(eve);
			}

			if (GetAsyncKeyState(VK_SPACE) && start_once)
			{
				start_once = false;
			}

			/*
				Update all ECS systems, and give them the delta time.
			*/

			//ecs.update(timer.GetFrameTime());
			ecs.update(1.f / 60.f);

			graphics::Present(1);
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
#pragma region FOR_DEBUGGING_DO_NOT_REMOVE
	//// FOR DEBBUGING TRAPS: SPAWNS TRAPS OVER THE WHOLE MAP
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
	//		place_event.type = (GAME_OBJECT_TYPES)((GAME_OBJECT_TYPE_TRAP_OFFSET_TAG + 1) + rand() % TRAP_TYPE_COUNT);
	//
	//		//if (place_event.type != GAME_OBJECT_TYPE_TRAP_FIRE)
	//		//{
	//		//	continue;
	//		//}
	//
	//		rECS.createEvent(place_event);
	//	}
	//}

	events::PlaceTrapEvent spawn_event;
	TypeFilter tile_filter;
	tile_filter.addRequirement(ecs::components::TileComponent::typeID);
	EntityIterator tiles = rECS.getEntititesByFilter(tile_filter);

	GAME_OBJECT_TYPES traps[] =
	{
		GAME_OBJECT_TYPE_TRAP_SPRING,
		//GAME_OBJECT_TYPE_WEAPON_SWORD,
		//GAME_OBJECT_TYPE_WEAPON_HAMMER,
	};

	int count = 0;
	for (FilteredEntity tile : tiles.entities)
	{
		components::TileComponent* p_tile = tile.getComponent<components::TileComponent>();
		if (p_tile->tileType != WATER /*&& (count % ((rand() % 4) + 8)) == 0*/)
		{
			spawn_event.type = traps[rand() % (sizeof(traps) / sizeof(GAME_OBJECT_TYPES))];// GAME_OBJECT_TYPES((rand() % TRAP_TYPE_COUNT) + (GAME_OBJECT_TYPE_TRAP_OFFSET_TAG + 1));
			spawn_event.tileID = p_tile->getEntityID();
			rECS.createEvent(spawn_event);
		}

		count++;
	}

	events::StartTrapSpawnSequenceEvent start_traps;
	start_traps.totalSpawnDuration = 1.0f;
	rECS.createEvent(start_traps);

	//events::SpawnWeaponEvent spawn_event;
	//TypeFilter tile_filter;
	//tile_filter.addRequirement(ecs::components::TileComponent::typeID);
	//EntityIterator tiles = rECS.getEntititesByFilter(tile_filter);
	//
	//GAME_OBJECT_TYPES traps[] =
	//{
	//	//GAME_OBJECT_TYPE_WEAPON_BOMB,
	//	//GAME_OBJECT_TYPE_WEAPON_SWORD,
	//	GAME_OBJECT_TYPE_TRAP_SPRING,
	//};
	//
	//int count = 0;
	//for (FilteredEntity tile : tiles.entities)
	//{
	//	components::TileComponent* p_tile = tile.getComponent<components::TileComponent>();
	//	if (p_tile->tileType != WATER /*&& (count % ((rand() % 4) + 8)) == 0*/)
	//	{
	//		spawn_event.weaponType = traps[rand() % (sizeof(traps) / sizeof(GAME_OBJECT_TYPES))];// GAME_OBJECT_TYPES((rand() % TRAP_TYPE_COUNT) + (GAME_OBJECT_TYPE_TRAP_OFFSET_TAG + 1));
	//		spawn_event.spawnTileId = p_tile->getEntityID();
	//		rECS.createEvent(spawn_event);
	//	}
	//
	//	count++;
	//}

	//// FOR WEAPON BALANCING, GIVES A FULL SET OF EACH WEAPON TO EACH PLAYER.
	//SpawnWeaponEvent spawn;
	//TypeFilter islet_tile_filter;
	//islet_tile_filter.addRequirement(IsletComponent::typeID);
	//islet_tile_filter.addRequirement(TileComponent::typeID);
	//islet_tile_filter.addRequirement(TransformComponent::typeID);
	//EntityIterator islet_tiles = rECS.getEntititesByFilter(islet_tile_filter);

	//std::vector<GAME_OBJECT_TYPE> types;
	//types.push_back(GAME_OBJECT_TYPE_WEAPON_SWORD);
	//types.push_back(GAME_OBJECT_TYPE_WEAPON_HAMMER);
	//types.push_back(GAME_OBJECT_TYPE_WEAPON_BOMB);
	//types.push_back(GAME_OBJECT_TYPE_WEAPON_FIST);

	//GAME_OBJECT_TYPE assigned[4] = { 0 };

	//// Assign weapon type randomly to teams.
	//int initial_size = types.size();
	//for (int i = 0; i < initial_size; i++)
	//{
	//	int index = rand() % types.size();
	//	assigned[i] = types[index];
	//	types.erase(types.begin() + index);
	//}

	//unordered_map<GAME_OBJECT_TYPE, int> weapon_counts; 
	//weapon_counts[GAME_OBJECT_TYPE_WEAPON_SWORD]	= 0;
	//weapon_counts[GAME_OBJECT_TYPE_WEAPON_FIST]		= 0;
	//weapon_counts[GAME_OBJECT_TYPE_WEAPON_HAMMER]	= 0;
	//weapon_counts[GAME_OBJECT_TYPE_WEAPON_BOMB]		= 0;

	//int2 grid_size = GridProp::GetInstance()->GetSize();

	//for (FilteredEntity islet_tile : islet_tiles.entities)
	//{
	//	IsletComponent* p_islet = islet_tile.getComponent<IsletComponent>();
	//	TileComponent* p_tile = islet_tile.getComponent<TileComponent>();
	//	TransformComponent* p_transform = islet_tile.getComponent<TransformComponent>();
	//	XMFLOAT3 pos = p_transform->position;
	//	int2 tile_index = GridFunctions::GetTileFromWorldPos(pos.x, pos.z);
	//	TileData tile_data = GridProp::GetInstance()->mGrid[tile_index.y][tile_index.x];
	//	
	//	UINT *neighbors = tile_data.neighbourIDArray;
	//	spawn.spawnTileId = 0;
	//	
	//	for (int i = 0; i < 6; i++)
	//	{
	//		ID current_id = neighbors[i];
	//		if ( current_id != 0
	//			&& rECS.getComponentFromEntity<TileComponent>(current_id)->tileType != WATER
	//			&& !rECS.getComponentFromEntity<IsletComponent>(current_id))
	//		{
	//			spawn.spawnTileId = current_id;
	//		}
	//	}

	//	spawn.weaponType = assigned[p_islet->playerId];
	//	if (weapon_counts[spawn.weaponType] < 3 && spawn.spawnTileId != 0)
	//	{
	//		weapon_counts[spawn.weaponType]++;
	//		rECS.createEvent(spawn);		
	//	}
	//}	

#pragma endregion
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

	InitIslands(rECS);

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
	//InitUI(rECS);

	InitWeapons(rECS);

	InitTraps(rECS);
	InitPowerups(rECS);

	InitWorldScenery(rECS);

	InitHttpServer(rECS);

	ecs::events::GameStartEvent eve;
	rECS.createEvent(eve);
	
	//DebuggFunctions(rECS);
}