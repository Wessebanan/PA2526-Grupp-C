#pragma once

#include "ecsSystemIncludes.h"

namespace ecs
{
	namespace systems
	{
		class PowerupLootAnimator : public ECSSystem<PowerupLootAnimator>
		{
		public:
			PowerupLootAnimator();
			~PowerupLootAnimator();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		};
	}
}