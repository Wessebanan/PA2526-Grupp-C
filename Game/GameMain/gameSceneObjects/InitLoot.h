#pragma once
#include "LootSystem.h"

void InitSpawnLootSystem(ecs::EntityComponentSystem &rEcs)
{
	rEcs.createSystem<ecs::systems::SpawnLootSystem>();
}