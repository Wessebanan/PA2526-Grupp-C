#pragma once
#include "ecs.h"
#include "AudioGlobals.h"
#include "AudioECSSystems.h"

// Creates the sound message system and init it
bool InitSound(ecs::EntityComponentSystem& rECS)
{
	rECS.reserveComponentCount<ecs::components::BattleMusicIntensityComponent>(1);
	ecs::components::BattleMusicIntensityComponent sound_dummy;
	rECS.createEntity(sound_dummy);


	ecs::systems::SoundMessageSystem* system_handler =
		rECS.createSystem<ecs::systems::SoundMessageSystem>(9);

	if (!system_handler->Init())
	{
		rECS.removeSystem<ecs::systems::SoundMessageSystem>();
		return false;
	}

	//rECS.createSystem<ecs::systems::SoundCooldownClearSystem>(9);

	rECS.createSystem<ecs::systems::BattleMusicIntensitySystem>()->Init();
	rECS.createSystem<ecs::systems::SubTrackUpdateSystem>();
	return true;
}

void InitSong(ecs::EntityComponentSystem& rECS)
{
	{
		ecs::events::PlayMusic m_event;
		m_event.audioName = AudioName::SOUND_cc_song;
		rECS.createEvent(m_event);
	}
	{
		ecs::events::MusicSetVolume m_event;
		m_event.volume = 0.0f;
		rECS.createEvent(m_event);
	}

	{
		ecs::events::PlaySecondaryMusic m_event;
		m_event.audioName = AudioName::SOUND_cc_chill;
		rECS.createEvent(m_event);
	}
	{
		ecs::events::SecondaryMusicSetVolume m_event;
		m_event.volume = 0.0f;
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

	{
		ecs::events::FadeInSecondaryMusic m_event;
		m_event.fadeInTimeInSeconds = 3.0f;
		rECS.createEvent(m_event);
	}
}