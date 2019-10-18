#pragma once
#include "ecs.h"
#include "AudioGlobals.h"
#include "AudioECSSystems.h"

// Creates the sound message system and init it
bool InitSound(ecs::EntityComponentSystem& rECS)
{
	ecs::systems::SoundMessageSystem* system_handler =
		rECS.createSystem<ecs::systems::SoundMessageSystem>(9);

	if (!system_handler->Init())
	{
		rECS.removeSystem<ecs::systems::SoundMessageSystem>();
		return false;
	}
	return true;
}
