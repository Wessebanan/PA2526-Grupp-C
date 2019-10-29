#pragma once
#include "ecsEventIncludes.h"


namespace ecs
{
	namespace events
	{
		// Event when the round should end,
		struct RoundEndEvent : public ecs::ECSEvent<RoundEndEvent>
		{
			int winner = -1; // The winner of the round 
		};

		// The round should start and create unit enteties
		struct RoundStartEvent : public ecs::ECSEvent<RoundStartEvent>
		{};

		// The game should start, timers and such
		struct GameStartEvent : public ecs::ECSEvent<GameStartEvent>
		{};
	}
}