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
	}
}