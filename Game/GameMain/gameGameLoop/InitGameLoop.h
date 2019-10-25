#pragma once
#include "ecs.h"
#include "GameLoopSystems.h"

void InitGameLoop(ecs::EntityComponentSystem& rECS)
{
	rECS.createSystem<ecs::systems::GameLoopSystem>();
}