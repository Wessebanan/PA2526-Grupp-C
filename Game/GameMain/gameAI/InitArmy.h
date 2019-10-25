#pragma once

#include "..//gameAI/AIComponents.h"
#include "AIEcsFunctions.h"

#include "..//gameUtility/UtilityComponents.h"
#include "ecs.h"

void InitArmy(ecs::EntityComponentSystem& rECS)
{
	AIEcsFunctions::CreatePlayerArmies(rECS);
}