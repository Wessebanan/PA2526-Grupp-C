#pragma once
#include "ecs.h"
#include "SoundGlobals.h"
#include "SoundECSSystems.h"

// Creates the sound message system and init it
bool InitSound(ecs::EntityComponentSystem& rECS)
{
	size_t hello = sizeof(ecs::systems::SoundMessageSystem);
	ecs::systems::SoundMessageSystem* system_handler =
		rECS.createSystem<ecs::systems::SoundMessageSystem>(9);

	if (!system_handler->Init())
	{
		return false;
	}
	return true;
}
