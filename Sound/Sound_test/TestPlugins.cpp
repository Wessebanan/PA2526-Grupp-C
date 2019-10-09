#include "pch.h"
#include "TestPlugins.h"

Sound::Plugin::TestSineWave::TestSineWave(float Frequency)
{
	mFrequency = Frequency;
}

Sound::Plugin::Status Sound::Plugin::TestSineWave::Process(Samples Start, Samples Count, float* Data, int Channels)
{
	const float AMPLITUDE = 0.2f;	// To save people's ears; Low amplitude
	float sample;
	Frame* frames = (Frame*)Data;
	for (Samples i = 0; i < Count; i++)
	{
		sample = std::sinf(mFrequency * Sound::ToSeconds(Start + i) * 2.0f * (float)M_PI) * AMPLITUDE;
		frames[i].left = sample;
		frames[i].right = sample;
	}
	return STATUS_OK;
}

Sound::Plugin::Passthrough::Passthrough(Plugin* pNext)
{
	mpNext = pNext;
}

Sound::Plugin::Status Sound::Plugin::Passthrough::Process(Samples Start, Samples Count, float* Data, int Channels)
{
	mpNext->Process(Start, Count, Data, Channels);
	return STATUS_OK;
}
