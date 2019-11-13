#pragma once
#include "ecs.h"
#include "TrapSystems.h"

void InitTrapTriggers(ecs::EntityComponentSystem& rECS)
{
	rECS.createSystem<ecs::systems::FireTrapEventSystem>(7);
	rECS.createSystem<ecs::systems::SpringTrapEventSystem>(7);
}