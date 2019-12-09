#include "InitTraps.h"

#include "TrapSpawnerSystems.h"
#include "TrapCollisionSystem.h"
#include "TrapSystems.h"

void InitTraps(ecs::EntityComponentSystem& rEcs)
{
	rEcs.createSystem<ecs::systems::TrapQueueSystem>(4);
	rEcs.createSystem<ecs::systems::TrapCollisionSystem>(4);

	rEcs.createSystem<ecs::systems::GenericTrapEventSystem>(5);

	rEcs.createSystem<ecs::systems::BurningDurationSystem>(5);
	rEcs.createSystem<ecs::systems::FireTrapEventSystem>(6);
	//rEcs.createSystem<ecs::systems::FreezeTrapEventSystem>(6);
	rEcs.createSystem<ecs::systems::SpikeTrapEventSystem>(6);
	rEcs.createSystem<ecs::systems::SpringTrapEventSystem>(6);
	rEcs.createSystem<ecs::systems::SpringRetractionSystem>(6);
}