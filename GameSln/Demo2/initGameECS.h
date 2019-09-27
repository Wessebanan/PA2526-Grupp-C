#pragma once
#include "ecs.h"
#include "GameSystems.h"

using namespace ecs;

bool initGameECS(ecs::EntityComponentSystem& ecs);

bool initGameECS(ecs::EntityComponentSystem& ecs)
{
	if (ecs.createSystem<systems::GameOverSystem>() == nullptr)
		return false;

	components::GameStateComponent gsComp = components::GameStateComponent();
	gsComp.gameState = 1;

	ecs.createEntity(
		gsComp
	);
}
