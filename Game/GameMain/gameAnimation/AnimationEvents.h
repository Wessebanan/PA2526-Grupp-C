#pragma once
#include "ecsEventIncludes.h"
#include "../../AI/includes/AIGlobals.h"

namespace ecs
{
	namespace events
	{
		// Holds the player responsible for the ping event
		struct PingEvent : public ecs::ECSEvent<PingEvent>
		{
			PLAYER playerId = PLAYER::PLAYER1;
		};
	}
}