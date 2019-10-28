#pragma once
#include "ecs.h"
#include "GameLoopSystems.h"

void InitGameLoop(ecs::EntityComponentSystem& rECS)
{
	rECS.createSystem<ecs::systems::GameLoopSystem>();
	rECS.createSystem<ecs::systems::GameLoopAliveSystem>(3);

	rECS.createSystem<ecs::systems::GameStartSystem>(5);
	rECS.createSystem<ecs::systems::RoundOverSystem>(6);
	rECS.createSystem<ecs::systems::RoundStartSystem>(7);



	ecs::components::GameLoopComponent gameloop_comp;
	gameloop_comp.mPlayerPoints[0] = 0;
	gameloop_comp.mPlayerPoints[1] = 0;
	gameloop_comp.mPlayerPoints[2] = 0;
	gameloop_comp.mPlayerPoints[3] = 0;
	ecs::components::GameSettingsComponent gameloop_settings_comp;
	gameloop_settings_comp.mMapSettings = 1; // Holme = 1

	rECS.createEntity(gameloop_comp, gameloop_settings_comp);
}