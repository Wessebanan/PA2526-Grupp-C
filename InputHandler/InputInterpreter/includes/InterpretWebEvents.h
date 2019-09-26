#pragma once
#include "ecsEventIncludes.h"
#include "AIGlobals.h"


namespace ecs
{
	namespace events
	{
		struct ChangeUserStateEvent : public ecs::ECSEvent<ChangeUserStateEvent>
		{
			PLAYER playerId = PLAYER::PLAYER1;
			STATE newState = STATE::IDLE;
		};
	}
}