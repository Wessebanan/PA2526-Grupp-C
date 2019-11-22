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

	rECS.createSystem<ecs::systems::SoundCooldownClearSystem>(9);
	return true;
}

void InitSong(ecs::EntityComponentSystem& rECS)
{
	{
		ecs::events::PlayMusic m_event;
		m_event.audioName = AudioName::SOUND_cc_chill;
		rECS.createEvent(m_event);
	}
	{
		ecs::events::MusicSetVolume m_event;
		m_event.volume = 0.0f;
		rECS.createEvent(m_event);
	}
	{
		ecs::events::FadeInMusic m_event;
		m_event.fadeInTimeInSeconds = 3.0f;
		rECS.createEvent(m_event);
	}

	{
		ecs::events::PlaySubMusic m_event;
		m_event.audioName = AudioName::SOUND_cc_drums;
		rECS.createEvent(m_event);
	}

	{
		ecs::events::SubMusicSetVolume m_event;
		m_event.volume = 0.0f;
		rECS.createEvent(m_event);
	}
}