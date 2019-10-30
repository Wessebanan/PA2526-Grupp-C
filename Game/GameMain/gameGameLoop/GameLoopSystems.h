#pragma once
#include "ecsSystemIncludes.h"
#include "GameLoopComponents.h"

#define ROUNDS_TO_WIN 3

namespace ecs
{
	namespace systems
	{
		// ----------- UPDATE SSTEMS ---------------

		// Updates for the current loop, not used for other than updates timers
		class GameLoopSystem : public ecs::ECSSystem<GameLoopSystem>
		{
		public:
			GameLoopSystem();
			~GameLoopSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

		// Checks if there is only one player alive then creates a evnt for it
		class GameLoopAliveSystem : public ecs::ECSSystem<GameLoopAliveSystem>
		{
		public:
			GameLoopAliveSystem();
			~GameLoopAliveSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};


		// ----------- EVENTREADERS -----------------

		// Starts the game, launches the correct phase after reading event
		class GameStartSystem : public ecs::ECSSystem<GameStartSystem>
		{
		public:
			GameStartSystem();
			~GameStartSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:
		};

		// Starts the round and initalizes the units and its components for each army
		class RoundStartSystem : public ecs::ECSSystem<RoundStartSystem>
		{
		public:
			RoundStartSystem();
			~RoundStartSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:
			void CreateUnits();
			void CreateUnitPhysics();
		};

		// Ends the round and sets a winner, also cheks if someone has won
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