#include "InitTrapsMACKE.h"

#include "TrapCollisionSystem.h"

void InitTraps(ecs::EntityComponentSystem& rEcs)
{
	rEcs.createSystem<ecs::systems::TrapSpawnerSystem>(4);
	rEcs.createSystem<ecs::systems::TrapCollisionSystem>(3);
}