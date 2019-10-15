#include "AudioMixer.h"

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
	mMusicManager.Fill(start, count, voice_data, 2);
	for (j = 0; j < count * 2; j++)
	{
		pData[j] += voice_data[j];
	}
	//for (i = 0; i < SOUND_MAX_MUSIC_VOICES; i++)
	//{
	//	if (mMusicVoices[i].IsActive())
	//	{
	//		mMusicVoices[i].Fill(start, count, voice_data);
	//		for (j = 0; j < count * 2; j++)
	//		{
	//			pData[j] += voice_data[j];
	//		}
	//	}
	//}
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

//void Audio::Music::Manager::NewMusicVoice(Plugin::Plugin* pEntryPlugin, bool replace)
//{
//	if (replace || !mMusicVoices[0].IsActive())
//	{
//		mMusicVoices[0].New(pEntryPlugin);
//		return;
//	}
//	// Failed to find space for the music, delete
//	delete pEntryPlugin;
//}

void Audio::Music::Manager::ReplaceMusic(Message& rMessage, MusicVoiceData* pTarget)
{
	if ((rMessage.flags & M_DATA_MASK) == M_DATA_AS_PARAMETER)
	{
		pTarget->Sampler.SetFileAndReset(rMessage.data.pFileData);
	}
}

void Audio::Mixer::AddSoundMessage(Sound::Message message)
{
	mSoundMessageBuffer.insert(&message);
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

void Audio::Music::Manager::ProcessMusicMessages()
{
	Music::Message temp_message;
	while (mMusicMessageBuffer.remove(&temp_message))
	{
		// Select target
		Manager::MusicVoiceData* target_data = nullptr;
		switch (temp_message.flags & M_TARGET_MASK)
		{
		case M_TARGET_MAIN:
			target_data = &mMainData;
			break;
		case M_TARGET_SUB:
			target_data = &mSubData;
			break;
		}

		// Call function
		switch (temp_message.flags & M_FUNC_MASK)
		{
		case M_FUNC_REPLACE_MUSIC:
			ReplaceMusic(temp_message, target_data);
			break;
		}

		// Data teardown
		if (temp_message.flags & M_DATA_TEARDOWN_MASK == M_DATA_TEARDOWN_DELETE)
		{
			delete temp_message.data.tpVoid;	// If it works like this
		}
	}
}

void Audio::Music::Manager::Fill(Samples start, Samples sampleCount, float* pData, int channelCount)
{
	int i, j;
	float voice_data[SOUND_FRAMES_PER_BUFFER * 2];

	mMainData.Entry->Process(start, sampleCount, voice_data, channelCount);
	for (j = 0; j < sampleCount * 2; j++)
	{
		pData[j] += voice_data[j];
	}

	mSubData.Entry->Process(start, sampleCount, voice_data, channelCount);
	for (j = 0; j < sampleCount * 2; j++)
	{
		pData[j] += voice_data[j];
	}
}

Audio::Music::Manager::Manager()
{
	mMainData.Entry = &mMainData.Sampler;
	mSubData.Entry = &mSubData.Sampler;
}

void Audio::Music::Manager::AddMusicMessage(Music::Message message)
{
	mMusicMessageBuffer.insert(&message);
}
