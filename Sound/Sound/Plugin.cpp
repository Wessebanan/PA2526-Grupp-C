#include "Plugin.h"
#include <cmath>

Sound::Plugin::Sampler::Sampler(FileData* pFile)
{
	mpFile = pFile;
	mReadPointer = 0;
}

// TODO: This sampler does not support mono wav files!
void Sound::Plugin::Sampler::Process(Samples start, Samples count, float* pData, int channels)
{
	// Get pointer to the data and amount of samples
	float* data_pointer = mpFile->GetDataPointer();
	Samples sample_count = mpFile->GetSampleCount();
	// For all frames that needs to be added...
	for (int i = 0; i < count; i++)
	{
		// For both channels...
		for (int j = 0; j < channels; j++)
		{
			// Read the data from the file
			*(pData++) = data_pointer[mReadPointer++];
			// If we've reached the end of the file
			if (mReadPointer >= sample_count)
			{
				// Go back to the start
				mReadPointer = 0;
			}
		}
	}
}
