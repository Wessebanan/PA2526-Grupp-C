#pragma once
#include "ecsSystemIncludes.h"
#include "TrapEvents.h"


/*
	-- HOW TO ADD A NEW TRAP --																						See file:

	1. Add trap tag in GAME_OBJECT_TYPE, must come after GAME_OBJECT_TYPE_TRAP_OFFSET_TAG in the 'trap block'.		GameGlobals.h
	2. Connect the trap's GAME_OBJECT_TYPE to a specific mesh in InitMeshes().										InitGraphics.h
	3. Create a Trigger____TrapEvent in TrapEvents for the new trap.												TrapEvents.h
	4. Add Trigger____TrapEvent creation in TrapCollisionSystem::updateEntity() switch case.						TrapCollisionSystem.h
	5. Add color for the trap's GAME_OBJECT_TYPE in switch case in TrapSpawnerSystem::readEvent().					TrapSpawnerSystem.cpp
	6. Create ____TrapEventSystem that handle the trap event.														TrapSystems.h / TrapSystems.cpp
*/

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
	}
}