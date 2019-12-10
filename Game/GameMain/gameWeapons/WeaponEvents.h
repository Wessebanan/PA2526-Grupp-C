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

		/*
			When created, a WeaponSequenceSpawnerSystem is created which will
			spawn random weapons in a sequence for its lifetime.
		*/
		struct StartWeaponSequenceEvent : public ECSEvent<StartWeaponSequenceEvent>
		{
			int spawnCount;
			float lifetime;
			bool createRoundStartEventWhenFinished = false;
		};
	}
}