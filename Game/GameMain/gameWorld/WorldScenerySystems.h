#pragma once

#include "ecsSystemIncludes.h"

namespace ecs
{
	namespace systems
	{
		class WorldSceneryUpdateSystem : public ECSSystem<WorldSceneryUpdateSystem>
		{
		public:
			WorldSceneryUpdateSystem();
			~WorldSceneryUpdateSystem();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		};
	}
}