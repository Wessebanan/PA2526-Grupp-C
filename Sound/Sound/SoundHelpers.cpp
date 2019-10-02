#include "SoundHelpers.h"

Sound::Samples Sound::ToSamples(const float Seconds)
{
	return (Samples)(Seconds * (float)SOUND_SAMPLE_RATE);
}

float Sound::ToSeconds(const Samples SampleCount)
{
	return (float)SampleCount / (float)SOUND_SAMPLE_RATE;
}