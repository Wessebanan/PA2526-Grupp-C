#pragma once
#include "ecsComponentIncludes.h"

namespace ecs
{
	namespace components
	{
		struct GameLoopComponent : public ecs::ECSComponent<GameLoopComponent>
		{
			bool mRoundOver;

			float mRoundTime;
			
			int mPlayerPoints[4];

		};

		struct GameSettingsComponent : public ecs::ECSComponent<GameSettingsComponent>
		{
			int mMapSettings;
		};
	}
}

