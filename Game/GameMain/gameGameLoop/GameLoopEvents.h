#pragma once
#include "ecsEventIncludes.h"

enum GAME_UI_TYPE
{
	GAME_UI_TYPE_UNDEFINED,
	GAME_UI_TYPE_OVERLAY,
	GAME_UI_TYPE_GUIDES
};

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

		// The game should start, timers and such
		struct GameReStartEvent : public ecs::ECSEvent<GameReStartEvent>
		{};

		// Enables or disables ui elements to be visible or not
		struct SetUIVisibilityEvent : public ECSEvent<SetUIVisibilityEvent>
		{
			GAME_UI_TYPE uiType;
			bool visible;

			SetUIVisibilityEvent(GAME_UI_TYPE type, bool visibility) :
				uiType(type), visible(visibility) {}
			SetUIVisibilityEvent() :
				uiType(GAME_UI_TYPE_UNDEFINED), visible(false) {}
		};
	}
}