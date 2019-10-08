#pragma once

#include "InterpretWebSystems.h"
#include "ecs.h"

void InitInterpreter(ecs::EntityComponentSystem& rECS)
{
	rECS.createSystem<ecs::systems::ChangeFSMSystem>();
}