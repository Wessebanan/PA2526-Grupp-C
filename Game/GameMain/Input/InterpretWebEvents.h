#pragma once
#include "ecsEventIncludes.h"
#include "../../AI/AI/includes/AIGlobals.h"


namespace ecs
{
	namespace events
	{
		// Holds the player and what state that the Interpreter has detected the the user wants to chnge to
		struct ChangeUserStateEvent : public ecs::ECSEvent<ChangeUserStateEvent>
		{
			PLAYER playerId = PLAYER::PLAYER1;
			STATE newState = STATE::IDLE;
		};
	}
}