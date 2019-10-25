#pragma once

#include "ecs.h"
#include "AISystems.h"

using namespace ecs;

void InitAI(EntityComponentSystem& rEcs)
{
	rEcs.createSystem<systems::PathfindingStateSystem>(5);
	rEcs.createSystem<systems::IdleStateSystem>(5);
	rEcs.createSystem<systems::MoveStateSystem>(5);
	rEcs.createSystem<systems::AttackStateSystem>(5);
	rEcs.createSystem<systems::SwitchStateSystem>(4);
	//ecs.createSystem<ecs::systems::DynamicMovementSystem>();
	//ecs.createSystem<ecs::systems::GroundCollisionSystem>();
}