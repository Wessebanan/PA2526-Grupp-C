#pragma once
#include "AudioPlugin.h"
#include "AudioVoice.h"
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
		enum MessageFlags : unsigned int
		{
			// Choose what this will modify
			M_TARGET_MAIN =				0,
			M_TARGET_SUB =				1,
			M_TARGET_MASK =				0b1,
			// What to do with the data pointer
			M_DATA_NO_DATA =			0,
			M_DATA_AS_PARAMETER =		(1 << 1),
			M_DATA_MASK =				(0b1 << 1),
			// If the data was a pointer, it may have to be deleted
			M_DATA_TEARDOWN_NOTHING =	0,
			M_DATA_TEARDOWN_DELETE =	(1 << 2),
			M_DATA_TEARDOWN_MASK =		(0b1 << 2),
			// Functions
			M_FUNC_NOTHING =			0,
			M_FUNC_REPLACE_MUSIC =		(1 << 3),
			M_FUNC_FADE_IN =			(2 << 3),
			M_FUNC_FADE_OUT =			(3 << 3),
			M_FUNC_MASK =				(0b11 << 3)

		};
		union MessageDataUnion
		{
			void* tpVoid;
			Audio::FileData* pFileData;
			float tFloat;
		};
		struct Message
		{
			MessageDataUnion data;
			unsigned int flags;
		};

		class Manager
		{
		public:
			Manager();
			void AddMusicMessage(Music::Message message);
			void ProcessMusicMessages();
			void Fill(Samples start, Samples sampleCount, float* pData, int channelCount);
		private:
			struct MusicVoiceData
			{
				Plugin::Plugin* Entry;
				Plugin::Sampler Sampler;
			};

			//void NewMusicVoice(Plugin::Plugin* pEntryPlugin, bool replace);
			void ReplaceMusic(Message& rMessage, MusicVoiceData* pTarget);

			Ringbuffer<Music::Message, SOUND_MAX_MUSIC_MESSAGES> mMusicMessageBuffer;
			//Voice mMusicVoices[SOUND_MAX_MUSIC_VOICES];
			MusicVoiceData mMainData;
			MusicVoiceData mSubData;
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
		//void ProcessMusicMessages();

		// Adds voices to sound and music respectively
		void NewSoundVoice(Plugin::Plugin* pEntryPlugin);
		

		Music::Manager mMusicManager;
		// Voices for sound and music
		Voice mSoundVoices[SOUND_MAX_SOUND_VOICES];
		// Voice mMusicVoices[SOUND_MAX_MUSIC_VOICES];
		// Ring buffer to contain sound and music messages
		Ringbuffer<Sound::Message, SOUND_MAX_SOUND_MESSAGES> mSoundMessageBuffer;
	};
}