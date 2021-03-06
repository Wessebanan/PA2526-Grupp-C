#pragma once
#include "ecsComponentIncludes.h"
#include "../gameUtility/Timer.h"

namespace ecs
{
	namespace components
	{
		// Holds points for each player and the Timer for the game
		struct GameLoopComponent : public ecs::ECSComponent<GameLoopComponent>
		{
			Timer mRoundTime;
			bool mGameOver;
			int mPlayerPoints[4];

		};

		// Can bee used to know what settign in on the game
		struct GameSettingsComponent : public ecs::ECSComponent<GameSettingsComponent>
		{
			int mMapSettings;
		};

		// Only used to fetch the components in the ECS
		struct GamePointsComponent : public ecs::ECSComponent<GamePointsComponent>
		{
		};
	}
}

