#pragma once
#include "ecsEventIncludes.h"
#include "../GameGlobals.h"

namespace ecs
{
	namespace events
	{
		// Event when the round should end,
		struct PlaceTrapEvent : public ecs::ECSEvent<PlaceTrapEvent>
		{
			TypeID tileID;
			GAME_OBJECT_TYPES type; // The winner of the round 
		};

	}
}