#include "LootSystem.h"

#pragma region SpawnLootSystem
ecs::systems::SpawnLootSystem::SpawnLootSystem()
{
	updateType = Actor;
	mTimeCounter = 0.0f;
}
ecs::systems::SpawnLootSystem::~SpawnLootSystem()
{

}
void ecs::systems::SpawnLootSystem::act(float _delta)
{
	mTimeCounter += _delta;

	// Truncating time to full seconds.
	int time_in_seconds = (int)mTimeCounter;

	// Once 10 seconds has passed, spawn a sword on random tile.
	if (time_in_seconds == 10)
	{	
		// Resetting time counter.

		mTimeCounter = 0.0f;

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