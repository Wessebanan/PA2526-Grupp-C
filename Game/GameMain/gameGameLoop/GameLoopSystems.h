#pragma once
#include "ecsSystemIncludes.h"
#include "GameLoopComponents.h"

namespace ecs
{
	namespace systems
	{
		class GameLoopSystem : public ecs::ECSSystem<GameLoopSystem>
		{
		public:
			GameLoopSystem();
			~GameLoopSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

		class GameLoopAliveSystem : public ecs::ECSSystem<GameLoopAliveSystem>
		{
		public:
			GameLoopAliveSystem();
			~GameLoopAliveSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};
	}
}
