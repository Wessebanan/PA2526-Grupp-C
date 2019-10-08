#pragma once
#include "Plugin.h"
#include "Voice.h"
namespace Sound
{
	struct SoundMessage
	{
		Plugin::Plugin* pEntry;
	};
	struct MusicMessage
	{
		Plugin::Plugin* pEntry;
	};
	class Mixer
	{
	public:
		// Fills the frame data with the total of all active voices
		void Fill(Samples Start, Samples Count, float* Data);

		void AddSoundMessage(SoundMessage rMessage);

		void ProcessMessages();
	private:
		void ProcessSoundMessages();
		void ProcessMusicMessages();
		// Adds a new voice if there's one available
		bool NewVoice(Plugin::Plugin* pEntryPlugin);
		Voice mVoices[SOUND_MAX_VOICES];
		Ringbuffer<SoundMessage, SOUND_MAX_SOUND_MESSAGES> mSoundMessageBuffer;
		Ringbuffer<MusicMessage, SOUND_MAX_MUSIC_MESSAGES> mMusicMessageBuffer;
	};
}