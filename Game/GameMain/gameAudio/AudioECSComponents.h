#pragma once

#include "ecsComponentIncludes.h"

namespace ecs
{
	namespace components
	{
		struct BattleMusicIntensityComponent : public ECSComponent<BattleMusicIntensityComponent>
		{
			float currentIntensity = 0.0f;

			float totalDistance = 0.0f;
			unsigned int totalCount = 0;
		};
	}
}