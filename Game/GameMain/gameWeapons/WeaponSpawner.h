#pragma once

#include "ecsSystemIncludes.h"

#include <vector>
#include "../GameGlobals.h"

namespace ecs
{
	namespace systems
	{

		/*
			-- WeaponSpawner
			System that acts on SpawnWeaponEvent.
		*/

		class WeaponSpawner : public ECSSystem<WeaponSpawner>
		{
		public:

			WeaponSpawner();
			~WeaponSpawner();

			void readEvent(BaseEvent& _event, float _delta) override;
		};

		/*
			-- Master Weapon Spawner
			System that creates spawn events for weapons.
		*/

		class MasterWeaponSpawner : public ECSSystem<MasterWeaponSpawner>
		{
		public:

			MasterWeaponSpawner();
			~MasterWeaponSpawner();

			void act(float _delta) override;

			void Initialize();

		private:

			float mSpawnTimer;
			const float SPAWN_OUTER_RADIUS = 8.f;
			const float SPAWN_INNER_RADIUS = 5.f;
			const float COOLDOWN_BASE_TIME = 9;
			const int COOLDOWN_TIME_VARIANCE = 3;

			std::vector<ID> mPossibleTileIds;

			ID FindSpawnTile();
			GAME_OBJECT_TYPE GetRandomWeaponType();
			void ResetSpawnTimer();

		};

		/*
			A system that removes the dynamic movement component from the weapon once it reaches its tile and 
			set its correct y-position and rotation according to the tile.
		*/
		class FallingWeaponSystem : public ECSSystem<FallingWeaponSystem>
		{
		public:
			FallingWeaponSystem();
			virtual ~FallingWeaponSystem();

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		/*
			A system with a limited life duration that spawn a sequence of weapons during its lifetime.
			Used in between prep phase and battle phase.
		*/
		class WeaponSequenceSpawnerSystem : public ECSSystem<WeaponSequenceSpawnerSystem>
		{
		public:
			WeaponSequenceSpawnerSystem();
			virtual ~WeaponSequenceSpawnerSystem();

			void Initialize();

			void act(float delta) override;
			void onEvent(TypeID _eventType, BaseEvent* _event) override;

		private:

			std::vector<ID> mPossibleTileIds;

			const float SPAWN_OUTER_RADIUS = 8.f;
			const float SPAWN_INNER_RADIUS = 5.f;

			int mSpawnCount = 0;
			float mLifeTime = 0.f;
			float mHaltDuration = 0.f;
			float mTimeSinceLastSpawn = 0.f;

			float mRoundStartCountdown = 0.f;
			bool mIsTimingForRoundStart = false;
			bool mCreateRoundStartEventWhenFinished = false;
			const float WAIT_TIME_BETWEEN_LAST_SPAWN_AND_ROUND_START = 2.5f;

			ID FindSpawnTile();
			GAME_OBJECT_TYPE GetRandomWeaponType();
		};
	}
}