#pragma once
#include "ecs.h"

namespace AIEcsFunctions
{
	//Creates the user entities for every player and their units entities.
	void CreatePlayerArmies(ecs::EntityComponentSystem& rEcs);
	unsigned int FindClosestEnemy(ecs::EntityComponentSystem& rEcs, ecs::Entity* current_unit);
}