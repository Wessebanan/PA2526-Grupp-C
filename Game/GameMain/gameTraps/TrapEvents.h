#pragma once
#include "ecsEventIncludes.h"
#include "../GameGlobals.h"

namespace ecs
{
	namespace events
	{
		struct PlaceTrapEvent : public ecs::ECSEvent<PlaceTrapEvent>
		{
			TypeID tileID;
			GAME_OBJECT_TYPES type; // The winner of the round 
		};

		// Trigger event for traps
		struct TriggerFireTrapEvent : public ecs::ECSEvent<TriggerFireTrapEvent>
		{
			TypeID unitID;
		};
		struct TriggerFreezeTrapEvent : public ecs::ECSEvent<TriggerFreezeTrapEvent>
		{
			TypeID unitID;
		};
		struct TriggerSpringTrapEvent : public ecs::ECSEvent<TriggerSpringTrapEvent>
		{
			TypeID unitID;
		};

	}
}