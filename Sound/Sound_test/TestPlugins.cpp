#include "pch.h"
#include "TestPlugins.h"

Audio::Plugin::TestSineWave::TestSineWave(float Frequency)
{
	mFrequency = Frequency;
}

Audio::Plugin::Status Audio::Plugin::TestSineWave::Process(Samples Start, Samples Count, float* Data, int Channels)
{
	const float AMPLITUDE = 0.2f;	// To save people's ears; Low amplitude
	float sample;
	Frame* frames = (Frame*)Data;
	for (Samples i = 0; i < Count; i++)
	{
		sample = std::sinf(mFrequency * Audio::ToSeconds(Start + i) * 2.0f * (float)M_PI) * AMPLITUDE;
		frames[i].left = sample;
		frames[i].right = sample;
	}
	return STATUS_OK;
}

Audio::Plugin::Passthrough::Passthrough(Plugin* pNext)
{
	mpNext = pNext;
}

Audio::Plugin::Status Audio::Plugin::Passthrough::Process(Samples Start, Samples Count, float* Data, int Channels)
{
	mpNext->Process(Start, Count, Data, Channels);
	return STATUS_OK;
}
