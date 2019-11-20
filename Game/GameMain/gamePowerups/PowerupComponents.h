#pragma once

#include "ecsComponentIncludes.h"
#include "../GameGlobals.h"

namespace ecs
{
	namespace components
	{
		struct PowerupLootComponent : public ECSComponent<PowerupLootComponent>
		{
			GAME_OBJECT_TYPE mPowerupType;
			uint32_t mColor;
		};

		struct HealthPackComponent : public ECSComponent<HealthPackComponent>
		{
			float mHealAmount;
		};
	}
}