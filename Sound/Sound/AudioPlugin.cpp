#include "AudioPlugin.h"
#include <cmath>

Audio::Plugin::Sampler::Sampler(FileData* pFile, int repeatAmount)
{
	mpFile = pFile;
	mRepeatAmount = repeatAmount;
	mReadPointer = 0;
}

Audio::Plugin::Sampler::Sampler()
{
	mpFile = nullptr;
	mRepeatAmount = 0;
	mReadPointer = 0;
}

void Audio::Plugin::Sampler::SetFileAndReset(FileData* pFile)
{
	mpFile = pFile;
	mReadPointer = 0;
}

Audio::Samples Audio::Plugin::Sampler::GetReadPointer()
{
	return mReadPointer;
}

void Audio::Plugin::Sampler::SetReadPointer(Samples readPointer)
{
	mReadPointer = readPointer;
}

// TODO: This sampler does not support mono wav files!
Audio::Plugin::Status Audio::Plugin::Sampler::Process(Samples start, Samples sampleCount, float* pData, int channelCount)
{

	int i = 0, j = 0;
	// If there's no file, don't play
	if (mpFile == nullptr)
	{
		goto LabelFinished;
	}
	// Get pointer to the data and amount of samples
	{
		float* data_pointer = mpFile->GetDataPointer();
		Samples sample_count = mpFile->GetSampleCount();
		// For all frames that needs to be added...
		for (; i < sampleCount; i++)
		{
			// For both channels...
			for (j = 0; j < channelCount; j++)
			{
				// Read the data from the file
				*(pData++) = data_pointer[mReadPointer++];
				// If we've reached the end of the file
				if (mReadPointer >= sample_count)
				{
					// Go back to the start
					mReadPointer = 0;
					// Signal to finish this voice if done
					// repeating
					if (mRepeatAmount == 1)
					{
						goto LabelFinished;
					}
					mRepeatAmount--;
				}
			}
		}
	}
	return STATUS_OK;
	// Fill the rest of the buffer with 0 and signal
	// that the plugin is finished and inactive
LabelFinished:
	for (; i < sampleCount; i++)
	{
		for (; j < channelCount; j++)
		{
			*(pData++) = 0.f;
		}
		j = 0;
	}
	return STATUS_FINISHED;
}

Audio::Plugin::Gain::Gain(Plugin* pNext, float gain)
{
	mpNext = pNext;
	mGain = gain;
}


void Audio::Plugin::Gain::SetGain(float gain)
{
	mGain = gain;
}

Audio::Plugin::Status Audio::Plugin::Gain::Process(Samples start, Samples sampleCount, float* pData, int channelCount)
{
	Status status = mpNext->Process(start, sampleCount, pData, channelCount);
	for (int i = 0; i < sampleCount * channelCount; i++)
	{
		pData[i] *= mGain;
	}
	return status;
}

void Audio::Plugin::Plugin::SetNextPointer(Plugin* pNext, bool NextIsOnStack)
{
	mpNext = pNext;
	mNextIsOnStack = NextIsOnStack;
}
