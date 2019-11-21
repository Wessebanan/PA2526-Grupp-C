#include "InitWeapons.h"
#include "WeaponSpawner.h"
#include "../Physics/PhysicsComponents.h"

using namespace ecs;

void InitWeapons(EntityComponentSystem& rEcs)
{
	rEcs.reserveComponentCount<components::WeaponComponent>(400);
	rEcs.createSystem<systems::WeaponSpawner>(5);
	rEcs.createSystem<systems::FallingWeaponSystem>(5);
}
