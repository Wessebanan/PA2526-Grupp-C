#pragma once

#include "..//gameAI/AIComponents.h"
#include "AIEcsFunctions.h"

#include "..//gameUtility/UtilityComponents.h"
#include "ecs.h"

void InitArmy(ecs::EntityComponentSystem& rECS)
{
	AIEcsFunctions::CreatePlayers(rECS);
	AIEcsFunctions::CreateUnits(rECS);
	PlayerStateComponent player_state_comp;
	rECS.createEntity(player_state_comp);
}