#pragma once
#include "ecs.h"
#include "AudioGlobals.h"
#include "AudioECSSystems.h"

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

	{
		ecs::events::PlayMusic m_event;
		m_event.audioName = AudioName::CC_TEST_SONG;
		rECS.createEvent(m_event);
	}

	{
		ecs::events::PlaySubMusic m_event;
		m_event.audioName = AudioName::CC_TEST_SONG;
		rECS.createEvent(m_event);
	}

	{
		ecs::events::SubMusicSetVolume m_event;
		m_event.volume = 0.0f;
		rECS.createEvent(m_event);
	}

	return true;
}
