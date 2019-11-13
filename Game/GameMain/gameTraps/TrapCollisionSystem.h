#pragma once

#include <DirectXMath.h>
#include "ecsSystemIncludes.h"

/*
	-- HOW TO ADD A NEW TRAP --																						See file:

	1. Add trap tag in GAME_OBJECT_TYPE, must come after GAME_OBJECT_TYPE_TRAP_OFFSET_TAG in the 'trap block'.		GameGlobals.h
	2. Connect the trap's GAME_OBJECT_TYPE to a specific mesh in InitMeshes().										InitGraphics.h
	3. Create a Trigger____TrapEvent in TrapEvents for the new trap.												TrapEvents.h
	4. Add Trigger____TrapEvent creation in TrapCollisionSystem::updateEntity() switch case.						TrapCollisionSystem.h
	5. Create ____TrapEventSystem that handle the trap event.														TrapSystems.h / TrapSystems.cpp
*/

namespace ecs
{
	namespace systems
	{
		/*
			-- TrapCollisionSystem
			Check collision between unit and traps. If a unit trigger a trap,
			this system creates an event for that trap type that corresponding
			trap trigger system will respond to.
		*/
		class TrapCollisionSystem : public ECSSystem<TrapCollisionSystem>
		{
		public:

			TrapCollisionSystem();
			~TrapCollisionSystem();

			void updateEntity(FilteredEntity& trap, float delta) override;

		private:

			const float TRIGGER_DISTANCE = 0.5f;
			const DirectX::XMFLOAT3 TRIGGER_POINT_OFFSET = { 0.f, 1.f, 0.f };
		};
	}
}