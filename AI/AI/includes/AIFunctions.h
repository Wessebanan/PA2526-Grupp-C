#pragma once
#include "ecs.h"

namespace AIFunctions
{
	//Creates the user entities for every player and their units entities.
	void CreatePlayerArmies(ecs::EntityComponentSystem& rEcs);
}