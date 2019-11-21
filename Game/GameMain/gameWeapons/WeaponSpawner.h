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
			const float SPAWN_RADIUS = 7.f;
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
	}
}