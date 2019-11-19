#pragma once

#include "ecsComponentIncludes.h"
#include "../GameGlobals.h"

namespace ecs
{
	namespace components
	{
		struct PowerupLootComponent : public ECSComponent<PowerupLootComponent>
		{
			GAME_OBJECT_TYPE mObjectType;
		};

		struct HealthPackComponent : public ECSComponent<HealthPackComponent>
		{
			float mHealAmount;
		};
	}
}