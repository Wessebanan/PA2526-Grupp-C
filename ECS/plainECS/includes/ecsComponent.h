
#pragma once

#include "ecsBaseComponent.h"

namespace ecs
{
	namespace components
	{
		struct HealthComponent : public ECSComponent<HealthComponent>
		{
			float health;
		};

		struct PoisonComponent : public ECSComponent<PoisonComponent>
		{
			float tickDamage;
		};
	}
}