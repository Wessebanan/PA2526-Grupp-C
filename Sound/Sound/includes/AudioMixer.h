#pragma once
#include "AudioPlugin.h"
#include "AudioVoice.h"
#include <complex>
#include <valarray>
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
			M_TARGET_SECONDARY =		1,
			M_TARGET_SUB =				2,
			M_TARGET_MASK =				0b11,
			// What to do with the data pointer
			M_DATA_NO_DATA =			0,
			M_DATA_AS_PARAMETER =		(1 << 2),
			M_DATA_MASK =				(0b1 << 2),
			// If the data was a pointer, it may have to be deleted
			M_DATA_TEARDOWN_NOTHING =	0,
			M_DATA_TEARDOWN_DELETE =	(1 << 3),
			M_DATA_TEARDOWN_MASK =		(0b1 << 3),
			// Syncs
			M_SYNC_NO_SYNC =			0,
			M_SYNC_THIS_WITH_OTHER =	(1 << 4),
			M_SYNC_OTHER_WITH_THIS =	(2 << 4),
			M_SYNC_MASK =				(0b11 << 4),
			// Functions
			M_FUNC_NOTHING =			0,
			M_FUNC_REPLACE_MUSIC =		(1 << 6),
			M_FUNC_SET_GAIN =			(2 << 6),
			M_FUNC_FADE_IN =			(3 << 6),
			M_FUNC_FADE_OUT =			(4 << 6),
			M_FUNC_SET_SPEED =			(5 << 6),
			M_FUNC_MASK =				(0b111 << 6)

		};
		union MessageDataUnion
		{
			MessageDataUnion()
			{
				_void = 0;
			}
			MessageDataUnion(float Float)
			{
				_float = Float;
			}
			MessageDataUnion(Audio::FileData* FileData)
			{
				_FileData = FileData;
			}
			MessageDataUnion(unsigned long Long)
			{
				_ulong = Long;
			}
			void* _void;
			Audio::FileData* _FileData;
			float _float;
			unsigned long _ulong;
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
			void Fill(Samples start, Samples sampleCount, float* pData, int channelCount, float* pVoiceData);
			Samples GetCurrentSampleFromMainMusic();
			void GetFrequencies(float* freqArray);
		private:
			struct MusicVoiceData
			{
				Plugin::Plugin* Entry;
				Plugin::Gain Gain;
				Plugin::Sampler Sampler;
			};

			//void NewMusicVoice(Plugin::Plugin* pEntryPlugin, bool replace);
			void ReplaceMusic(Message& rMessage, MusicVoiceData* pTarget);
			void SetGain(Message& rMessage, MusicVoiceData* pTarget);
			void FadeIn(Message& rMessage, MusicVoiceData* pTarget);
			void FadeOut(Message& rMessage, MusicVoiceData* pTarget);
			void SetSpeed(Message& rMessage, MusicVoiceData* pTarget);


			Ringbuffer<Music::Message, SOUND_MAX_MUSIC_MESSAGES> mMusicMessageBuffer;
			Ringbuffer<float, 256> mPreviousSamples;
			//Voice mMusicVoices[SOUND_MAX_MUSIC_VOICES];
			MusicVoiceData mMainData;
			MusicVoiceData mSecondaryData;
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
		// Get current sample of the main music playing
		Samples GetCurrentSampleFromMainMusic();

		Music::Manager* GetMusicManager();
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