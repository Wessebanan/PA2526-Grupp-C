#include "AudioMixer.h"
#include <iostream>

void Audio::Mixer::Fill(Samples start, Samples count, float* pData)
{
	int i, j;
	float voice_data[SOUND_FRAMES_PER_BUFFER * 2] = {0.0f};
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

	mMusicManager.Fill(start, count, pData, 2, voice_data);
}


void Audio::Mixer::NewSoundVoice(Plugin::Plugin* pEntryPlugin)
{
	for (int i = 0; i < SOUND_MAX_SOUND_VOICES; i++)
	{
		if (!mSoundVoices[i].IsActive())
		{
			mSoundVoices[i].New(pEntryPlugin);
			return;
		}
	}
	// Failed to find space for the sound, delete
	delete pEntryPlugin;
}

void Audio::Mixer::AddSoundMessage(Sound::Message message)
{
	if (!mSoundMessageBuffer.insert(&message))
	{
		delete message.pEntry;
	}
}

void Audio::Mixer::AddMusicMessage(Music::Message message)
{
	mMusicManager.AddMusicMessage(message);
}

void Audio::Mixer::ProcessMessages()
{
	if (!mSoundMessageBuffer.isEmpty())
	{
		ProcessSoundMessages();
	}
	//if (!mMusicMessageBuffer.isEmpty())
	//{
	mMusicManager.ProcessMusicMessages();
	//}
}

void Audio::Mixer::ProcessSoundMessages()
{
	Sound::Message temp_message;
	while (mSoundMessageBuffer.remove(&temp_message))
	{
		NewSoundVoice(temp_message.pEntry);
	}
}