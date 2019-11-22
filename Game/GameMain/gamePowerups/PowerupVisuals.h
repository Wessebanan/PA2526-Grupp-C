#pragma once

#include "ecsSystemIncludes.h"

namespace ecs
{
	namespace systems
	{
		class PowerupLootAnimatorSystem : public ECSSystem<PowerupLootAnimatorSystem>
		{
		public:
			PowerupLootAnimatorSystem();
			~PowerupLootAnimatorSystem();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		};
	}
}