#include "InitPowerups.h"
#include "PowerupSpawner.h"
#include "PowerupEvents.h"
#include "PowerupComponents.h"
#include "PowerupVisuals.h"

using namespace ecs;
using namespace ecs::events;
using namespace ecs::systems;
using namespace ecs::components;

void InitPowerups(ecs::EntityComponentSystem& rEcs)
{
	rEcs.reserveComponentCount<PowerupLootComponent>(400);
	rEcs.reserveComponentCount<HealthPackComponent>(400);

	rEcs.createSystem<PowerupSpawner>(5);
	rEcs.createSystem<PowerupLootAnimator>(5);
}
