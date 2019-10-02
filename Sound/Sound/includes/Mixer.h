#pragma once
#include "Plugin.h"
#include "Voice.h"
namespace Sound
{
	class Mixer
	{
	public:
		// Fills the frame data with the total of all active voices
		void Fill(Samples Start, Samples Count, float* Data);
		// Adds a new voice if there's one available
		bool NewVoice(Plugin::Plugin* pEntryPlugin);
	private:
		Voice mVoices[SOUND_MAX_VOICES];
	};
}