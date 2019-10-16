#include "AudioMixer.h"

void Audio::Music::Manager::ReplaceMusic(Message& rMessage, MusicVoiceData* pTarget)
{
	if ((rMessage.flags & M_DATA_MASK) == M_DATA_AS_PARAMETER)
	{
		pTarget->Sampler.SetFileAndReset(rMessage.data.pFileData);
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
		if ((temp_message.flags & M_DATA_TEARDOWN_MASK) == M_DATA_TEARDOWN_DELETE)
		{
			delete temp_message.data.tpVoid;	// If it works like this
		}
	}
}

void Audio::Music::Manager::Fill(Samples start, Samples sampleCount, float* pData, int channelCount, float* pVoiceData)
{
	int i, j;
	//float voice_data[SOUND_FRAMES_PER_BUFFER * 2];

	mMainData.Entry->Process(start, sampleCount, pVoiceData, channelCount);
	for (j = 0; j < sampleCount * 2; j++)
	{
		pData[j] += pVoiceData[j];
	}

	mSubData.Entry->Process(start, sampleCount, pVoiceData, channelCount);
	for (j = 0; j < sampleCount * 2; j++)
	{
		pData[j] += pVoiceData[j];
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
