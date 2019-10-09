#pragma once
#include "ecs.h"
#include "SoundGlobals.h"
#include "SoundECSSystems.h"

bool InitSound(ecs::EntityComponentSystem& rECS)
{
	size_t hello = sizeof(ecs::systems::SoundMessageSystem);
	ecs::systems::SoundMessageSystem* system_handler =
		rECS.createSystem<ecs::systems::SoundMessageSystem>(0);

	if (!system_handler->Init())
	{
		return false;
	}
	return true;
}
