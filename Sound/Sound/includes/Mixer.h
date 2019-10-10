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
		// Flags to signal what should be done with the plugin
		enum MessageFlags
		{
			M_NONE = 0x0,
			M_REPLACE = 0x1
		};
		struct Message
		{
			Plugin::Plugin* pEntry;
			MessageFlags flags;
		};
	}
	class Mixer
	{
	public:
		// Fills the frame data with the total of all active voices
		void Fill(Samples start, Samples count, float * pData);

		// Functions for adding messages
		void AddSoundMessage(Sound::Message message);
		void AddMusicMessage(Music::Message message);

		// Processing sound and music messages
		void ProcessMessages();
	private:
		void ProcessSoundMessages();
		void ProcessMusicMessages();

		// Adds voices to sound and music respectively
		void NewSoundVoice(Plugin::Plugin* pEntryPlugin);
		void NewMusicVoice(Plugin::Plugin* pEntryPlugin, bool replace);

		// Voices for sound and music
		Voice mSoundVoices[SOUND_MAX_SOUND_VOICES];
		Voice mMusicVoices[SOUND_MAX_MUSIC_VOICES];
		// Ring buffer to contain sound and music messages
		Ringbuffer<Sound::Message, SOUND_MAX_SOUND_MESSAGES> mSoundMessageBuffer;
		Ringbuffer<Music::Message, SOUND_MAX_MUSIC_MESSAGES> mMusicMessageBuffer;
	};
}