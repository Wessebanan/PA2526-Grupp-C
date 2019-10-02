#pragma once
#include "Plugin.h"
#include "Voice.h"
namespace Sound
{
	class Mixer
	{
	public:
		void Fill(Samples Start, Samples Count, float* Data);
		bool NewVoice(Plugin::Plugin* pEntryPlugin);
	private:
		Voice mVoices[SOUND_MAX_VOICES];
	};
}