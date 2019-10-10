#pragma once
#include "Plugin.h"
#include "Voice.h"
namespace Audio
{
	namespace Sound
	{

		struct Message
		{
			Plugin::Plugin* pEntry;
		};
	}
	namespace Music
	{
		struct Message
		{
			Plugin::Plugin* pEntry;
		};
	}
	class Mixer
	{
	public:
		// Fills the frame data with the total of all active voices
		void Fill(Samples start, Samples count, float * pData);

		void AddSoundMessage(Sound::Message message);
		void AddMusicMessage(Music::Message message);

		void ProcessMessages();
	private:
		void ProcessSoundMessages();
		void ProcessMusicMessages();
		// Adds a new voice if there's one available
		bool NewSoundVoice(Plugin::Plugin* pEntryPlugin);
		bool NewMusicVoice(Plugin::Plugin* pEntryPlugin);
		Voice mSoundVoices[SOUND_MAX_SOUND_VOICES];
		Voice mMusicVoices[SOUND_MAX_MUSIC_VOICES];
		Ringbuffer<Sound::Message, SOUND_MAX_SOUND_MESSAGES> mSoundMessageBuffer;
		Ringbuffer<Music::Message, SOUND_MAX_MUSIC_MESSAGES> mMusicMessageBuffer;
	};
}