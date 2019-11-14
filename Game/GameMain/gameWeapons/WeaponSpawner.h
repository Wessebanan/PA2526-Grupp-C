#pragma once

#include "ecsSystemIncludes.h"

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

		private:

			const float SPAWN_FREQUENCY = 1.f;
			float mDurationSinceLastSpawn;

			ID FindSpawnTile();

			EntityIterator mTiles;
		};
	}
}