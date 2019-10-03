#include "Mixer.h"

void Sound::Mixer::Fill(Samples Start, Samples Count, float* Data)
{
	int i, j;
	float voice_data[SOUND_FRAMES_PER_BUFFER*2];
	for (i = 0; i < SOUND_MAX_VOICES; i++)
	{
		if (mVoices[i].IsActive())
		{
			mVoices[i].Fill(Start, Count, voice_data);
			for (j = 0; j < Count*2; j++)
			{
				Data[j] += voice_data[j];
			}
		}
	}
}

bool Sound::Mixer::NewVoice(Plugin::Plugin* pEntryPlugin)
{
	for (int i = 0; i < SOUND_MAX_VOICES; i++)
	{
		if (!mVoices[i].IsActive())
		{
			mVoices[i].New(pEntryPlugin);
			return true;
		}
	}
	return false;
}
