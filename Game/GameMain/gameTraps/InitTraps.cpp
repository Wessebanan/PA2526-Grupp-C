#include "InitTraps.h"

void InitTrapSpawner(ecs::EntityComponentSystem& rEcs)
{
	rEcs.createSystem<ecs::systems::TrapSpawnerSystem>(4);
}