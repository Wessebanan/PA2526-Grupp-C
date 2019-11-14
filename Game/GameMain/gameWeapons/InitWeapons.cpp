#include "InitWeapons.h"
#include "WeaponSpawner.h"
#include "../Physics/PhysicsComponents.h"

using namespace ecs;

void InitWeapons(EntityComponentSystem& rEcs)
{
	rEcs.reserveComponentCount<components::WeaponComponent>(400);
	rEcs.reserveComponentCount<components::MeshComponent>(400);

	rEcs.createSystem<systems::MasterWeaponSpawner>(2);
	rEcs.createSystem<systems::WeaponSpawner>(5);
}
