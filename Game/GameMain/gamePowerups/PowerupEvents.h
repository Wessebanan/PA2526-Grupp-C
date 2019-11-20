#pragma once

#include "ecsEventIncludes.h"
#include "../GameGlobals.h"

namespace ecs
{
	namespace events
	{
		struct SpawnPowerupEvent : public ECSEvent<SpawnPowerupEvent>
		{
			GAME_OBJECT_TYPE powerupType;
			ID spawnTileId;
		};

		struct PowerupTriggerEvent : public ECSEvent<PowerupTriggerEvent>
		{
			GAME_OBJECT_TYPE powerupType;
			ID affectedUnitId;
		};
	}
}