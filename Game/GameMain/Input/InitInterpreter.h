#pragma once

#include "ecs.h"
#include "InterpretWebSystems.h"

void InitInterpreter(ecs::EntityComponentSystem& rECS)
{
	rECS.createSystem<ecs::systems::ChangeFSMSystem>(3);
}