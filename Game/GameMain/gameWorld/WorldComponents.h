#pragma once

#include "ecsComponentIncludes.h"
#include "../GameGlobals.h"

namespace ecs
{
	namespace components
	{
		struct OceanTileComponent : public ECSComponent<OceanTileComponent>
		{

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