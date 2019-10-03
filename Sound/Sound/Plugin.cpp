#include "Plugin.h"
#include <cmath>

Sound::Plugin::Sampler::Sampler(FileData* pFile)
{
	mpFile = pFile;
	mReadPointer = 0;
}

// TODO: This sampler does not support mono wav files!
void Sound::Plugin::Sampler::Process(Samples Start, Samples Count, float* Data, int Channels)
{
	// Get pointer to the data and amount of samples
	float* data_pointer = mpFile->GetDataPointer();
	Samples sample_count = mpFile->GetSampleCount();
	// For all frames that needs to be added...
	for (int i = 0; i < Count; i++)
	{
		// For both channels...
		for (int j = 0; j < Channels; j++)
		{
			// Read the data from the file
			*(Data++) = data_pointer[mReadPointer++];
			// If we've reached the end of the file
			if (mReadPointer >= sample_count)
			{
				// Go back to the start
				mReadPointer = 0;
			}
		}
	}
}
