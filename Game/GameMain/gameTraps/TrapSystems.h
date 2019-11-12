#pragma once
#include "ecsSystemIncludes.h"
#include "TrapEvents.h"

namespace ecs
{
	namespace systems
	{

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
			const float mPower = 0.5f;
		};
	}
}