#include "AudioPlugin.h"
#include <cmath>

Audio::Plugin::Sampler::Sampler(FileData* pFile, int repeatAmount, float rateModifier)
{
	mpFile = pFile;
	mRepeatAmount = repeatAmount;
	mReadPointer = 0;
	mReadFraction = 0.0f;
	mPlayRate = rateModifier;
}

Audio::Plugin::Sampler::Sampler()
{
	mpFile = nullptr;
	mRepeatAmount = 0;
	mReadPointer = 0;
	mReadFraction = 0.0f;
	mPlayRate = 1.0f;
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

void Audio::Plugin::Sampler::SetPlayRate(float playRate)
{
	mPlayRate = playRate;
}

Audio::Plugin::Status Audio::Plugin::Sampler::Progress(Samples start, Samples sampleCount, int channelCount)
{
	float temp_float;
	mReadFraction = modf(
		mReadFraction + mPlayRate * sampleCount,
		&temp_float);
	mReadPointer += (Samples)temp_float * channelCount;
	//mReadPointer += sampleCount * channelCount;

	Samples sample_count = mpFile->GetSampleCount();
	if (mReadPointer >= sample_count)
	{
		// Go back to the start
		mReadPointer %= sample_count;
		// Signal to finish this voice if done
		// repeating
		if (mRepeatAmount == 1)
		{
			return Status::STATUS_FINISHED;
		}
		mRepeatAmount--;
	}
	return Status::STATUS_OK;
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
				// ---  Interpolation  ---
				// Get the next sample to interpolate towards
				// and wrap if outside the file
				Samples next_sample =
					(mReadPointer + j + channelCount) % sample_count;

				// Read the data from the file and interpolate
				*(pData++) =
					data_pointer[mReadPointer + j] * (1.0f - mReadFraction)
					+ data_pointer[next_sample] * mReadFraction;
			}
			// Progress the sound with the play rate
			float temp_float;
			mReadFraction = modf(
				mReadFraction + mPlayRate,
				&temp_float);
			mReadPointer += (Samples)temp_float * channelCount;
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
	mGainSpeed = 0.0f;
}


void Audio::Plugin::Gain::SetGain(float gain)
{
	mGain = gain;
}

void Audio::Plugin::Gain::FadeToFull(unsigned long sampleDuration)
{
	mGainSpeed = (1.0f - mGain) / sampleDuration;
}

void Audio::Plugin::Gain::FadeToEmpty(unsigned long sampleDuration)
{
	mGainSpeed = (-mGain) / sampleDuration;
}

Audio::Plugin::Status Audio::Plugin::Gain::Process(Samples start, Samples sampleCount, float* pData, int channelCount)
{
	Status status;
	// If the gain is 0, just fill with 0, so sound will be playing
	if (mGain == 0.0f)
	{
		status = mpNext->Progress(start, sampleCount, channelCount);
		for (int i = 0; i < sampleCount; i++)
		{
			for (int j = 0; j < channelCount; j++)
			{
				pData[i * 2 + j] = 0.0f;
			}
		}
	}
	// Else, process and adjust gain
	else
	{
		status = mpNext->Process(start, sampleCount, pData, channelCount);
		for (int i = 0; i < sampleCount; i++)
		{
			for (int j = 0; j < channelCount; j++)
			{
				pData[i*2+j] *= mGain + mGainSpeed * i;
			}
		}
	}
	// Interpolate the gain and clamp if necessary
	mGain += mGainSpeed * sampleCount;
	if (mGain > 1.0f)
	{
		mGain = 1.0f;
		mGainSpeed = 0.0f;
	}
	if (mGain < 0.0f)
	{
		mGain = 0.0f;
		mGainSpeed = 0.0f;
	}
	return status;
}

void Audio::Plugin::Plugin::SetNextPointer(Plugin* pNext, bool NextIsOnStack)
{
	mpNext = pNext;
	mNextIsOnStack = NextIsOnStack;
}

Audio::Plugin::Status Audio::Plugin::Plugin::Progress(Samples start, Samples sampleCount, int channelCount)
{
	return Status::STATUS_OK;
}
