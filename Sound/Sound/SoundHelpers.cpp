#include "SoundHelpers.h"

Sound::Samples Sound::ToSamples(const float seconds)
{
	return (Samples)(seconds * (float)SOUND_SAMPLE_RATE);
}

float Sound::ToSeconds(const Samples sampleCount)
{
	return (float)sampleCount / (float)SOUND_SAMPLE_RATE;
}