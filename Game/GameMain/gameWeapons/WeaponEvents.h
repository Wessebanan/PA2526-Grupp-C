#pragma once

#include "ecsEventIncludes.h"
#include "../GameGlobals.h"

namespace ecs
{
	namespace events
	{
		/*
			-- SpawnWeaponEvent
			Event for spawning a weapon of given type, at given tile.
		*/

		struct SpawnWeaponEvent : public ECSEvent<SpawnWeaponEvent>
		{
			GAME_OBJECT_TYPE weaponType;
			ID spawnTileId;
		};
	}
}