#pragma once
#include "ecsSystemIncludes.h"
#include "TrapEvents.h"

namespace ecs
{
	namespace systems
	{

		// ----------- EVENTREADERS -----------------

		// Starts the game, launches the correct phase after reading event
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