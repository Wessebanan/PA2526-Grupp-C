#pragma once

#include "ecsComponentIncludes.h"
#include "../GameGlobals.h"

namespace ecs
{
	namespace components
	{
		struct OceanTileComponent : public ECSComponent<OceanTileComponent>
		{
			unsigned int mWaveIndex = 999999;
			float mWaveAmplifier = 0.0f;
		};

		struct WaveCenterComponent : public ECSComponent<WaveCenterComponent>
		{
			float* mpFirstElement;
		};

		struct IsletComponent : public ECSComponent<IsletComponent>
		{
			ID playerId;
		};

		struct WorldSceneryComponent : public ECSComponent<WorldSceneryComponent>
		{
			GAME_OBJECT_TYPE sceneryType;
			unsigned int mWaveIndex;
		};
	}
}