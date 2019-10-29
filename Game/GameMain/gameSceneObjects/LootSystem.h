#pragma once
#include "ecs.h"
#include "..//Physics/PhysicsComponents.h"

#define SYSTEM(name) struct name : public ecs::ECSSystem<name>

SYSTEM(SpawnLootSystem)
{
	SpawnLootSystem();
	~SpawnLootSystem();

};