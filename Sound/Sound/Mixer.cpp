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

void Audio::Mixer::NewMusicVoice(Plugin::Plugin* pEntryPlugin, bool replace)
{
	if (replace || !mMusicVoices[0].IsActive())
	{
		mMusicVoices[0].New(pEntryPlugin);
		return;
	}
	// Failed to find space for the music, delete
	delete pEntryPlugin;
}

void Audio::Mixer::AddSoundMessage(Sound::Message message)
{
	mSoundMessageBuffer.insert(&message);
}

void Audio::Mixer::AddMusicMessage(Music::Message message)
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
	Sound::Message temp_message;
	while (mSoundMessageBuffer.remove(&temp_message))
	{
		NewSoundVoice(temp_message.pEntry);
	}
}

void Audio::Mixer::ProcessMusicMessages()
{
	Music::Message temp_message;
	while (mMusicMessageBuffer.remove(&temp_message))
	{
		NewMusicVoice(temp_message.pEntry,
			temp_message.flags & Music::M_REPLACE);
	}
}
