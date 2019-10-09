#include "Mixer.h"

void Sound::Mixer::Fill(Samples start, Samples count, float * pData)
{
	int i, j;
	float voice_data[SOUND_FRAMES_PER_BUFFER*2];
	for (i = 0; i < SOUND_MAX_SOUND_VOICES; i++)
	{
		if (mSoundVoices[i].IsActive())
		{
			mSoundVoices[i].Fill(start, count, voice_data);
			for (j = 0; j < count*2; j++)
			{
				pData[j] += voice_data[j];
			}
		}
	}
	for (i = 0; i < SOUND_MAX_MUSIC_VOICES; i++)
	{
		if (mMusicVoices[i].IsActive())
		{
			mMusicVoices[i].Fill(start, count, voice_data);
			for (j = 0; j < count * 2; j++)
			{
				pData[j] += voice_data[j];
			}
		}
	}
}

bool Sound::Mixer::NewSoundVoice(Plugin::Plugin* pEntryPlugin)
{
	for (int i = 0; i < SOUND_MAX_SOUND_VOICES; i++)
	{
		if (!mSoundVoices[i].IsActive())
		{
			mSoundVoices[i].New(pEntryPlugin);
			return true;
		}
	}
	return false;
}

bool Sound::Mixer::NewMusicVoice(Plugin::Plugin* pEntryPlugin)
{
	if (!mMusicVoices[0].IsActive())
	{
		mMusicVoices[0].New(pEntryPlugin);
		return true;
	}
	return false;
}

void Sound::Mixer::AddSoundMessage(SoundMessage rMessage)
{
	mSoundMessageBuffer.insert(&rMessage);
}

void Sound::Mixer::ProcessMessages()
{
	if (!mSoundMessageBuffer.isEmpty())
	{
		ProcessSoundMessages();
	}
	if (!mMusicMessageBuffer.isEmpty())
	{
		ProcessMusicMessages();
	}
}

void Sound::Mixer::ProcessSoundMessages()
{
	SoundMessage temp_message;
	while (mSoundMessageBuffer.remove(&temp_message))
	{
		NewSoundVoice(temp_message.pEntry);
	}
}

void Sound::Mixer::ProcessMusicMessages()
{
	// Music messages are currently nothing different
	// from sound messages.
	// Will be changed in a future task
	MusicMessage temp_message;
	while (mMusicMessageBuffer.remove(&temp_message))
	{
		NewMusicVoice(temp_message.pEntry);
	}
}
