#include "LootSystem.h"

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

	// Once 10 seconds has passed, spawn a sword on random tile.
	if (round_time_in_seconds == mTargetTime)
	{	
		// Bumping target time by 10 to spawn every 10 seconds.
		mTargetTime += 10;

		WeaponComponent		weapon_component;
		TransformComponent	weapon_transform_component;
		MeshComponent		weapon_mesh_component;
		ColorComponent		color_comp;

		weapon_component.mType = SWORD;
		weapon_mesh_component.mMesh = MeshContainer::GetMeshCPU(MESH_TYPE_SWORD);
		weapon_transform_component.scale = XMFLOAT3(0.1f, 0.1f, 0.1f);

		Entity *sword = createEntity(weapon_mesh_component, weapon_transform_component, weapon_component, color_comp);
		TransformComponent* sword_transform = getComponentFromKnownEntity<TransformComponent>(sword->getID());

		GridProp* grid = GridProp::GetInstance();
		int2 grid_size = grid->GetSize();		
		TileData random_tile;
		random_tile.isPassable = false;
		
		// Until a passable tile is found that does not already have loot.
		while (!random_tile.isPassable)
		{
			random_tile = grid->mGrid[rand() % grid_size.y][rand() % grid_size.x];
			
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

		// Place the sword on chosen tile's position.
		TransformComponent* tile_transform = getComponentFromKnownEntity<TransformComponent>(random_tile.Id);
		sword_transform->position = tile_transform->position;
	}
}
#pragma endregion