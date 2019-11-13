#include "InitTraps.h"

#include "TrapSpawnerSystems.h"
#include "TrapCollisionSystem.h"
#include "TrapSystems.h"

void InitTraps(ecs::EntityComponentSystem& rEcs)
{
	rEcs.createSystem<ecs::systems::TrapSpawnerSystem>(1);
	rEcs.createSystem<ecs::systems::TrapCollisionSystem>(1);

	rEcs.createSystem<ecs::systems::FreezingDurationSystem>(3);
	rEcs.createSystem<ecs::systems::FireTrapEventSystem>(7);
	rEcs.createSystem<ecs::systems::FreezeTrapEventSystem>(7);
}