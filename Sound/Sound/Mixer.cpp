#include "Mixer.h"

void Audio::Mixer::Fill(Samples start, Samples count, float* pData)
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

bool Audio::Mixer::NewSoundVoice(Plugin::Plugin* pEntryPlugin)
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

bool Audio::Mixer::NewMusicVoice(Plugin::Plugin* pEntryPlugin)
{
	if (!mMusicVoices[0].IsActive())
	{
		mMusicVoices[0].New(pEntryPlugin);
		return true;
	}
	return false;
}

void Audio::Mixer::AddSoundMessage(SoundMessage message)
{
	mSoundMessageBuffer.insert(&message);
}

void Audio::Mixer::AddMusicMessage(MusicMessage message)
{
	mMusicMessageBuffer.insert(&message);
}

void Audio::Mixer::ProcessMessages()
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

void Audio::Mixer::ProcessSoundMessages()
{
	SoundMessage temp_message;
	while (mSoundMessageBuffer.remove(&temp_message))
	{
		NewSoundVoice(temp_message.pEntry);
	}
}

void Audio::Mixer::ProcessMusicMessages()
{
	MusicMessage temp_message;
	while (mMusicMessageBuffer.remove(&temp_message))
	{
		NewMusicVoice(temp_message.pEntry);
	}
}
