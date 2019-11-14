#pragma once
#include "ecsSystemIncludes.h"
#include "TrapEvents.h"
#include "TrapComponentsLHURE.h"
#include <vector>

namespace ecs
{
	namespace systems
	{
		// ----------- UPDATE SYSTEMS ---------------

		// Updates the timer on frozzen durations
		class FreezingDurationSystem : public ecs::ECSSystem<FreezingDurationSystem>
		{
		public:
			FreezingDurationSystem();
			~FreezingDurationSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

		// ----------- EVENTREADERS -----------------

		class FireTrapEventSystem : public ecs::ECSSystem<FireTrapEventSystem>
		{
		public:
			FireTrapEventSystem();
			~FireTrapEventSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:

			const float mDamage = 40.0f;
			const float mKnockback = 80.0f;
		};

		class FreezeTrapEventSystem : public ecs::ECSSystem<FreezeTrapEventSystem>
		{
		public:
			FreezeTrapEventSystem();
			~FreezeTrapEventSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:

			const float mDamage = 10.0f;
			const float mPower = 0.5f; // REMEMBER TO SYNC WITH FREEZING DURATION STSTEM
		};


		// Starts the game, launches the correct phase after reading event
		class SpringTrapEventSystem : public ecs::ECSSystem<SpringTrapEventSystem>
		{
		public:
			SpringTrapEventSystem();
			~SpringTrapEventSystem();
			void readEvent(BaseEvent& event, float delta) override;
		private:

			const float mKnockback = 10.0f;
		};
	}
}