#pragma once
#include "ecsEventIncludes.h"
#include "../GameGlobals.h"


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
	namespace events
	{
		struct PlaceTrapEvent : public ecs::ECSEvent<PlaceTrapEvent>
		{
			ID userID = 0;
			TypeID tileID;
			GAME_OBJECT_TYPES type; // The winner of the round
		};

		// Generic trap trigger event for collision.
		struct TriggerTrapEvent : public ecs::ECSEvent<TriggerTrapEvent>
		{
			GAME_OBJECT_TYPE trapType;

			ID unitID;
			ID tileID;
			ID trapID;
		};

		// Specific trigger events for traps
		struct StartTrapSpawnSequenceEvent : public ECSEvent<StartTrapSpawnSequenceEvent>
		{
			float totalSpawnDuration;
		};

		// Trigger event for traps
		struct TriggerFireTrapEvent : public ecs::ECSEvent<TriggerFireTrapEvent>
		{
			ID unitID;
			ID tileID;
			ID trapID;
		};
		struct TriggerFreezeTrapEvent : public ecs::ECSEvent<TriggerFreezeTrapEvent>
		{
			ID unitID;
			ID tileID;
			ID trapID;
		};
		struct TriggerSpringTrapEvent : public ecs::ECSEvent<TriggerSpringTrapEvent>
		{
			ID unitID;
			ID tileID;
			ID trapID;
		};
		struct TriggerSpikeTrapEvent : public ecs::ECSEvent<TriggerSpikeTrapEvent>
		{
			ID unitID;
			ID tileID;
			ID trapID;
		};
	}
}