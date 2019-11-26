#pragma once

#include "ecs.h"
#include "InterpretWebSystems.h"

void InitInterpreter(ecs::EntityComponentSystem& rECS)
{
	rECS.createSystem<ecs::systems::ChangeFSMSystem>(3);
	rECS.createSystem<ecs::systems::TrapEventSystem>(3);
}