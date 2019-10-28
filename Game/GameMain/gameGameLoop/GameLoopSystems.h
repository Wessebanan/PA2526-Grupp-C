#pragma once
#include "ecsSystemIncludes.h"
#include "GameLoopComponents.h"

namespace ecs
{
	namespace systems
	{
		// ----------- UPDATE SSTEMS ---------------

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


		// ----------- EVENTREADERS -----------------

		class GameStartSystem : public ecs::ECSSystem<GameStartSystem>
		{
		public:
			GameStartSystem();
			~GameStartSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:
		};

		class RoundStartSystem : public ecs::ECSSystem<RoundStartSystem>
		{
		public:
			RoundStartSystem();
			~RoundStartSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:
		};

		class RoundOverSystem : public ecs::ECSSystem<RoundOverSystem>
		{
		public:
			RoundOverSystem();
			~RoundOverSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:
		};
	}
}
