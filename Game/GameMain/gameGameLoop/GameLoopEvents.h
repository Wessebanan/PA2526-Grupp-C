#pragma once
#include "ecsEventIncludes.h"


namespace ecs
{
	namespace events
	{
		struct RoundEndEvent : public ecs::ECSEvent<RoundEndEvent>
		{
			int winner = -1; // The winner of hte round 
		};

		struct RoundStartEvent : public ecs::ECSEvent<RoundStartEvent>
		{};

		struct GameStartEvent : public ecs::ECSEvent<GameStartEvent>
		{};
	}
}