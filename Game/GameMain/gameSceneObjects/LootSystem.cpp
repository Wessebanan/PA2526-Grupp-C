#include "LootSystem.h"
#include "../gameWeapons/WeaponEvents.h"

#include "../gameGraphics/ParticleECSComponents.h"

#pragma region SpawnLootSystem
ecs::systems::SpawnLootSystem::SpawnLootSystem()
{
	updateType = Actor;
	mTargetTime = 0;
	mLastRoundTime = 0;
}
ecs::systems::SpawnLootSystem::~SpawnLootSystem()
{

}
void ecs::systems::SpawnLootSystem::act(float _delta)
{
	int round_time_in_seconds = 0;

	// Grab round timer from game loop component.
	ComponentIterator itt = getComponentsOfType<GameLoopComponent>();
	GameLoopComponent* p_gl;
	while (p_gl = (GameLoopComponent*)itt.next())
	{
		round_time_in_seconds = (int)p_gl->mRoundTime.GetRoundTime();
	}

	// Should only trigger if a new round is started.
	if (round_time_in_seconds < mLastRoundTime)
	{
		mTargetTime = 0;
	}
	mLastRoundTime = round_time_in_seconds;

	GridProp* grid = GridProp::GetInstance();
	// Once 10 seconds has passed, spawn a sword on random tile.
	if (round_time_in_seconds == mTargetTime && grid->mLootTiles.size() < 12)
	{	
		// Bumping target time by 10 to spawn every 10 seconds.
		mTargetTime += 10;

		int2 grid_size = grid->GetSize();		
		TileData random_tile;
		random_tile.isPassable = false;
		
		// Until a passable tile is found that does not already have loot.
		while (!random_tile.isPassable)
		{
			int2 random_tile_index;
			random_tile_index.x = (rand() % (grid_size.x / 2)) + grid_size.x / 4;
			random_tile_index.y = (rand() % (grid_size.y / 2)) + grid_size.y / 4;

			random_tile = grid->mGrid[random_tile_index.y][random_tile_index.x];
			
			// Setting the COPY of TileData to not passable if there is already loot there
			// to continue the search for a suitable tile.
			int i = 0;
			for (int i = 0; i < grid->mLootTiles.size(); i++)
			{
				if (random_tile.Id == grid->mLootTiles[i])
				{
					random_tile.isPassable = false;
				}
			}
		}
		grid->mLootTiles.push_back(random_tile.Id);

		events::SpawnWeaponEvent spawn_event;
		spawn_event.weaponType = GAME_OBJECT_TYPE_WEAPON_SWORD;
		spawn_event.spawnTileId = random_tile.Id;
		createEvent(spawn_event);
	}
}
#pragma endregion