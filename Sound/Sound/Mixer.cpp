#include "Mixer.h"

void Sound::Mixer::Fill(Samples Start, Samples Count, float* Data)
{
	int i, j;
	float voice_data[SOUND_FRAMES_PER_BUFFER*2];
	for (i = 0; i < SOUND_MAX_VOICES; i++)
	{
		if (mVoices[i].IsActive())
		{
			mVoices[i].Fill(Start, Count, voice_data);
			for (j = 0; j < Count*2; j++)
			{
				Data[j] += voice_data[j];
			}
		}
	}
}

bool Sound::Mixer::NewVoice(Plugin::Plugin* pEntryPlugin)
{
	for (int i = 0; i < SOUND_MAX_VOICES; i++)
	{
		if (!mVoices[i].IsActive())
		{
			mVoices[i].New(pEntryPlugin);
			return true;
		}
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
		NewVoice(temp_message.pEntry);
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
		NewVoice(temp_message.pEntry);
	}
}
