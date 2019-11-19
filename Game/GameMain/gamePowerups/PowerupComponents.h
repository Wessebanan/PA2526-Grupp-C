#pragma once

#include "ecsComponentIncludes.h"

namespace ecs
{
	namespace components
	{
		struct PowerupLootComponent : public ECSComponent<PowerupLootComponent>
		{

		};

		struct HealthPackComponent : public ECSComponent<HealthPackComponent>
		{
			float healAmount;
		};
	}
}