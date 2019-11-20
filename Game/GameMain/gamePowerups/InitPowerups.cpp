#include "InitPowerups.h"
#include "PowerupSpawner.h"
#include "PowerupEvents.h"
#include "PowerupComponents.h"
#include "PowerupVisuals.h"
#include "PowerupLogic.h"

using namespace ecs;
using namespace ecs::events;
using namespace ecs::systems;
using namespace ecs::components;

void InitPowerups(ecs::EntityComponentSystem& rEcs)
{
	rEcs.reserveComponentCount<PowerupLootComponent>(400);
	rEcs.reserveComponentCount<HealthPackComponent>(400);

	rEcs.createSystem<PowerupSpawner>(1);
	rEcs.createSystem<PowerupLootAnimatorSystem>(1);
	rEcs.createSystem<PowerupLootCollisionSystem>(1);

	rEcs.createSystem<HealthPackTriggerSystem>(3);
}
