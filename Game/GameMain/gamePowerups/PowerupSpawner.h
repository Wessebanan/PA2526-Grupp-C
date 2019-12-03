#pragma once

#include "ecsSystemIncludes.h"

namespace ecs
{
	namespace systems
	{
		class PowerupSpawner : public ECSSystem<PowerupSpawner>
		{
		public:

			PowerupSpawner();
			~PowerupSpawner();

			void readEvent(BaseEvent& _event, float _delta) override;
		};
	}
}