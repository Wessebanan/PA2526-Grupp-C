#include "SoundHelpers.h"

Audio::Samples Audio::ToSamples(const float seconds)
{
	return (Samples)(seconds * (float)SOUND_SAMPLE_RATE);
}

float Audio::ToSeconds(const Samples sampleCount)
{
	return (float)sampleCount / (float)SOUND_SAMPLE_RATE;
}