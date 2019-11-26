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
	rEcs.createSystem<systems::FleeStateSystem>(5);
	rEcs.createSystem<systems::LootStateSystem>(5);
	rEcs.createSystem<systems::RemoveDeadUnitsSystem>(9);
	rEcs.createSystem<systems::PotentialWaterHazardSystem>(3);
	rEcs.createSystem<systems::PotentialArmyHazardSystem>(3);
	rEcs.createSystem<systems::AIPlayerSystem>(3);
}