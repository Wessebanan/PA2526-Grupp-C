#pragma once
#include "ecsEventIncludes.h"
#include "TrapGlobals.h"

namespace ecs
{
	namespace events
	{
		// Event when the round should end,
		struct PlaceTrapEvent : public ecs::ECSEvent<PlaceTrapEvent>
		{
			TypeID tileID;
			TRAPTYPES type; // The winner of the round 
		};

	}
}