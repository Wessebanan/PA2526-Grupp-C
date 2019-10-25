#pragma once
#include "ecsComponentIncludes.h"#include "../gameUtility/Timer.h"

namespace ecs
{
	namespace components
	{
		struct GameLoopComponent : public ecs::ECSComponent<GameLoopComponent>
		{
			bool mRoundOver;

			Timer mRoundTime;
			
			int mPlayerPoints[4];

		};

		struct GameSettingsComponent : public ecs::ECSComponent<GameSettingsComponent>
		{
			int mMapSettings;
		};
	}
}

