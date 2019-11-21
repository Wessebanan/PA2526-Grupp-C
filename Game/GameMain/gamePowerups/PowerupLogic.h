#pragma once

#include "ecsSystemIncludes.h"

namespace ecs
{
	namespace systems
	{
		/*
			-- PowerupLootCollisionSystem
			This system searches for collision between powerups and units. If 
			a collision is found, this system removes the powerup loot entity
			and create a trigger event for the powerup type and the effected
			unit for other systems to handle.

			In order to change how close a unit has to be to a powerup in order
			to pick it up, change PICKUP_RANGE in this system class.
		*/
		class PowerupLootCollisionSystem : public ECSSystem<PowerupLootCollisionSystem>
		{
		public:

			PowerupLootCollisionSystem();
			~PowerupLootCollisionSystem();

			void updateMultipleEntities(EntityIterator& rPowerups, float delta) override;

		private:

			const float PICKUP_RANGE = 0.7f;
		};

		/*
			Below are all systems that handle powerup triggers.
		*/

		class HealthPackTriggerSystem : public ECSSystem<HealthPackTriggerSystem>
		{
		public:

			HealthPackTriggerSystem();
			~HealthPackTriggerSystem();

			void readEvent(BaseEvent& _event, float _delta) override;
		};

		class PowerupSpeedTriggerSystem : public ECSSystem<PowerupSpeedTriggerSystem>
		{
		public:

			PowerupSpeedTriggerSystem();
			~PowerupSpeedTriggerSystem();

			void readEvent(BaseEvent& _event, float _delta) override;
		};
	}
}