#pragma once

#include "ecsSystemIncludes.h"

namespace ecs
{
	namespace systems
	{
		class TrapSpawnerSystem : public ECSSystem<TrapSpawnerSystem>
		{
		public:

			TrapSpawnerSystem();
			virtual ~TrapSpawnerSystem();

			void readEvent(BaseEvent& _event, float _delta) override;

		private:
		};
	}
}