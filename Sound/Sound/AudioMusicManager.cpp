#include "AudioMixer.h"

void Audio::Music::Manager::ReplaceMusic(Message& rMessage, MusicVoiceData* pTarget)
{
	if ((rMessage.flags & M_DATA_MASK) == M_DATA_AS_PARAMETER)
	{
		pTarget->Sampler.SetFileAndReset(rMessage.data._FileData);
	}
}

void Audio::Music::Manager::SetGain(Message& rMessage, MusicVoiceData* pTarget)
{
	if ((rMessage.flags & M_DATA_MASK) == M_DATA_AS_PARAMETER)
	{
		pTarget->Gain.SetGain(rMessage.data._float);
	}
}

void Audio::Music::Manager::FadeIn(Message& rMessage, MusicVoiceData* pTarget)
{
	if ((rMessage.flags & M_DATA_MASK) == M_DATA_AS_PARAMETER)
	{
		pTarget->Gain.FadeToFull(rMessage.data._ulong);
	}
}

void Audio::Music::Manager::FadeOut(Message& rMessage, MusicVoiceData* pTarget)
{
	if ((rMessage.flags & M_DATA_MASK) == M_DATA_AS_PARAMETER)
	{
		pTarget->Gain.FadeToEmpty(rMessage.data._ulong);
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
		case M_TARGET_SECONDARY:
			target_data = &mSecondaryData;
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
		case M_FUNC_SET_GAIN:
			SetGain(temp_message, target_data);
			break;
		case M_FUNC_FADE_IN:
			FadeIn(temp_message, target_data);
			break;
		case M_FUNC_FADE_OUT:
			FadeOut(temp_message, target_data);
			break;
		}

		// Syncing (Could be done more elegantly)
		if ((temp_message.flags & M_SYNC_MASK) == M_SYNC_THIS_WITH_OTHER)
		{
			switch (temp_message.flags & M_TARGET_MASK)
			{
			case M_TARGET_MAIN:
				mMainData.Sampler.SetReadPointer(
					mSubData.Sampler.GetReadPointer());
				break;
			case M_TARGET_SECONDARY:
				mSecondaryData.Sampler.SetReadPointer(
					mMainData.Sampler.GetReadPointer());
				break;
			case M_TARGET_SUB:
				mSubData.Sampler.SetReadPointer(
					mMainData.Sampler.GetReadPointer());
				break;
			}
		}

		if ((temp_message.flags & M_SYNC_MASK) == M_SYNC_OTHER_WITH_THIS)
		{
			switch (temp_message.flags & M_TARGET_MASK)
			{
			case M_TARGET_MAIN:
				mSubData.Sampler.SetReadPointer(
					mMainData.Sampler.GetReadPointer());
				break;
			case M_TARGET_SUB:
				mMainData.Sampler.SetReadPointer(
					mSubData.Sampler.GetReadPointer());
				break;
			}
		}

		// Data teardown
		if ((temp_message.flags & M_DATA_TEARDOWN_MASK) == M_DATA_TEARDOWN_DELETE)
		{
			delete temp_message.data._void;	// If it works like this
		}
	}
}

void Audio::Music::Manager::Fill(Samples start, Samples sampleCount, float* pData, int channelCount, float* pVoiceData)
{
	int i, j;

	mMainData.Entry->Process(start, sampleCount, pVoiceData, channelCount);
	for (j = 0; j < sampleCount * 2; j++)
	{
		pData[j] += pVoiceData[j];
	}

	mSecondaryData.Entry->Process(start, sampleCount, pVoiceData, channelCount);
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
	mMainData.Gain.SetNextPointer(&mMainData.Sampler, true);
	mMainData.Entry = &mMainData.Gain;

	mSecondaryData.Gain.SetNextPointer(&mSecondaryData.Sampler, true);
	mSecondaryData.Entry = &mSecondaryData.Gain;
	
	mSubData.Gain.SetNextPointer(&mSubData.Sampler, true);
	mSubData.Entry = &mSubData.Gain;
}

void Audio::Music::Manager::AddMusicMessage(Music::Message message)
{
	mMusicMessageBuffer.insert(&message);
}
