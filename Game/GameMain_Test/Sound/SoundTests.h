#pragma once
#include "gtest/gtest.h"
#include "ecs.h"
#include "InitAudio.h"
TEST(SoundAPI, PlayAudioThroughECS)
{
	ecs::EntityComponentSystem ecs;

	InitSound(ecs);
	// TEMPORARY
	// Just to play something to know it works
	ecs::events::PlaySound sound_event;
	sound_event.audioName = AudioName::COIN_TEST_SOUND;
	sound_event.soundFlags = SoundFlags::SF_REPEAT;
	ecs.createEvent(sound_event);

	ecs::events::PlayMusic music_event;
	music_event.audioName = AudioName::CC_TEST_SONG;
	music_event.musicFlags = MusicFlags::MF_REPLACE;
	ecs.createEvent(music_event);

	int loop_amount = 50;
	while (loop_amount != 0)
	{
		ecs.update(0.1f);
		Pa_Sleep(100);
		loop_amount--;
	}
}