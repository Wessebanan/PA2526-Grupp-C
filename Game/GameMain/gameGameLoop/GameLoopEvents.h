#pragma once
#include "ecsEventIncludes.h"


namespace ecs
{
	namespace events
	{
		// Holds the player and what state that the Interpreter has detected the the user wants to chnge to
		struct RoundEvent : public ecs::ECSEvent<RoundEvent>
		{
			
		};
	}
}