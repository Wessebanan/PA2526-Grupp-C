#pragma once
#include "ecsSystemIncludes.h"
#include "GameLoopComponents.h"
#include "..//Input/InitInputBackendComponent.h"
#include "Mesh.h"
#include "..//Physics/PhysicsComponents.h"
#include "..//GameGlobals.h"

#define ROUNDS_TO_WIN 2

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

		// Checks for transition to bettlephase
		class WaitForStartupSystem : public ecs::ECSSystem<WaitForStartupSystem>
		{
		public:
			WaitForStartupSystem();
			~WaitForStartupSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

		// Checks for transition to battlephase
		class PrepPhaseSystem : public ecs::ECSSystem<PrepPhaseSystem>
		{
		public:
			PrepPhaseSystem();
			~PrepPhaseSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

		// CHecks for transition to prepphase
		class BattlePhaseSystem : public ecs::ECSSystem<BattlePhaseSystem>
		{
		public:
			BattlePhaseSystem();
			~BattlePhaseSystem();
			void updateMultipleEntities(EntityIterator &_entities, float _delta) override;
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

			// Creates a weapon out of a mesh and weapon type. (weapon, transform and mesh components)
			ecs::Entity* CreateWeaponEntity(ModelLoader::Mesh* pMesh, GAME_OBJECT_TYPE weaponType, ID ownerEntity = 0);
		};

		// Ends the round and sets a winner, also cheks if someone has won
		class RoundOverSystem : public ecs::ECSSystem<RoundOverSystem>
		{
		public:
			RoundOverSystem();
			~RoundOverSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:
			bool mRoundOver;
			float mRoundOverDuration;
		};
	}
}
