#pragma once

#include "ecsSystemIncludes.h"

namespace ecs
{
	namespace systems
	{
		class PowerupLootCollisionSystem : public ECSSystem<PowerupLootCollisionSystem>
		{
		public:

			PowerupLootCollisionSystem();
			~PowerupLootCollisionSystem();

			void updateMultipleEntities(EntityIterator& rPowerups, float delta) override;

		private:

			const float PICKUP_RANGE = 1.f;
		};
	}
}