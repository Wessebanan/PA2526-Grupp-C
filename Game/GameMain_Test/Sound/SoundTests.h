#pragma once
#include "gtest/gtest.h"
#include "ecs.h"
#include "InitAudio.h"
//TEST(SoundAPI, PlayAudioThroughECS)
//{
//	ecs::EntityComponentSystem ecs;
//
//	ASSERT_TRUE(InitSound(ecs)) << "InitSound Failed!";
//
//
//	ecs::events::PlaySound sound_event;
//	sound_event.audioName = AudioName::COIN_TEST_SOUND;
//	sound_event.soundFlags = SoundFlags::SF_REPEAT;
//	ecs.createEvent(sound_event);
//
//	ecs::events::PlayMusic music_event;
//	music_event.audioName = AudioName::CC_TEST_SONG;
//	ecs.createEvent(music_event);
//
//	ecs::events::MusicSetVolume music_volume_event;
//	music_volume_event.volume = 0.0f;
//	ecs.createEvent(music_volume_event);
//
//	ecs::events::FadeInMusic fade_in_event;
//	fade_in_event.fadeInTimeInSeconds = 3.0f;
//	ecs.createEvent(fade_in_event);
//
//
//	ecs::events::PlaySubMusic sub_music_event;
//	sub_music_event.audioName = AudioName::CC_TEST_SONG_DRUMS;
//	ecs.createEvent(sub_music_event);
//
//	ecs::events::SubMusicSetVolume sub_music_volume_event;
//	sub_music_volume_event.volume = 0.0f;
//	ecs.createEvent(sub_music_volume_event);
//
//	ecs::events::FadeInSubMusic fade_in_sub_event;
//	fade_in_sub_event.fadeInTimeInSeconds = 0.5f;
//	ecs.createEvent(fade_in_sub_event);
//
//	int loop_amount = 50;
//	while (loop_amount != 0)
//	{
//		ecs.update(0.1f);
//		Pa_Sleep(100);
//		loop_amount--;
//		if (loop_amount == 25)
//		{
//			ecs::events::FadeOutSubMusic fade_out_sub_event;
//			fade_out_sub_event.fadeOutTimeInSeconds = 1.5f;
//			ecs.createEvent(fade_out_sub_event);
//		}
//	}
//}
//
//TEST(SoundAPI, SpamMessages)
//{
//	ecs::EntityComponentSystem ecs;
//
//	ASSERT_TRUE(InitSound(ecs)) << "InitSound Failed!";
//
//	ecs::events::PlaySound sound_event;
//	sound_event.audioName = AudioName::SILENCE_SOUND;
//	sound_event.soundFlags = SoundFlags::SF_NONE;
//	for (int i = 0; i < 10000; i++)
//	{
//		ecs.createEvent(sound_event);
//	}
//
//	ecs::events::PlayMusic music_event;
//	music_event.audioName = AudioName::SILENCE_SOUND;
//	for (int i = 0; i < 10000; i++)
//	{
//		ecs.createEvent(music_event);
//	}
//
//	int loop_amount = 15;
//	while (loop_amount != 0)
//	{
//		ecs.update(0.1f);
//		Pa_Sleep(100);
//		loop_amount--;
//	}
//}

TEST(SoundAPI, PlayAudioThroughECS)
{
	ecs::EntityComponentSystem ecs;

	ASSERT_TRUE(InitSound(ecs)) << "InitSound Failed!";

	ecs::events::PlayMusic music_event;
	music_event.audioName = AudioName::CC_TEST_SONG;
	ecs.createEvent(music_event);

	ecs::events::MusicSetVolume music_volume_event;
	music_volume_event.volume = 1.0f;
	ecs.createEvent(music_volume_event);

	ecs::events::PlaySubMusic sub_music_event;
	sub_music_event.audioName = AudioName::CC_TEST_SONG;
	ecs.createEvent(sub_music_event);

	ecs::events::SubMusicSetVolume sub_music_volume_event;
	sub_music_volume_event.volume = 0.0f;
	ecs.createEvent(sub_music_volume_event);

	int loop_amount = 50;
	while (loop_amount != 0)
	{
		ecs.update(0.1f);
		Pa_Sleep(100);
		loop_amount--;
		if (loop_amount == 25)
		{
			// TEMP GROUND HIT SOUND
			ecs::events::PlaySound sound_event;
			sound_event.audioName = AudioName::COIN_TEST_SOUND;
			sound_event.soundFlags = SoundFlags::SF_NONE;
			ecs.createEvent(sound_event);

			{
				ecs::events::FadeInSubMusic m_event;
				m_event.fadeInTimeInSeconds = 3.0f;
				ecs.createEvent(m_event);
			}
		}
	}
}