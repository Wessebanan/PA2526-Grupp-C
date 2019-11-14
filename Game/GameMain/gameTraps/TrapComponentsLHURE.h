#pragma once
#include "ecsComponentIncludes.h"

namespace ecs
{
	namespace components
	{
		// Holds the time of how long the unit has been frozzen and the max time
		struct FreezingTimerComponent: public ecs::ECSComponent<FreezingTimerComponent>
		{
			float mDuration;

			float mElapsedTime = 0;
		};
	}
}

