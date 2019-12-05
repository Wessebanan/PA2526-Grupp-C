#pragma once
#include "AudioHelpers.h"
#include "AudioFile.h"

namespace Audio
{
	namespace Plugin
	{
		enum Status
		{
			STATUS_OK,
			STATUS_FINISHED
		};
		// Base plugin structure
		class Plugin
		{
		public:
			Plugin()
			{
				mpNext = nullptr;
				mNextIsOnStack = false;
			}
			// Once one plugin gets deleted, the chain of
			// plugins gets deleted
			~Plugin() {
				if (!mNextIsOnStack && mpNext != nullptr)
				{
					delete mpNext;
					mpNext = nullptr;
				}
			}
			void SetNextPointer(Plugin* pNext, bool NextIsOnStack);
			virtual Status Progress(Samples start, Samples sampleCount, int channelCount);
			virtual Status Process(Samples start, Samples sampleCount, float* pData, int channelCount) = 0;
		protected:
			Plugin* mpNext;
			bool mNextIsOnStack;
		};

		class Sampler : public Plugin
		{
		public:
			Sampler(FileData* pFile, int repeatAmount, float playRate = 1.0f);
			Sampler();
			void SetFileAndReset(FileData* pFile);
			Samples GetReadPointer();
			void SetReadPointer(Samples readPointer);
			void SetPlayRate(float playRate);
			virtual Status Progress(Samples start, Samples sampleCount, int channelCount);
			virtual Status Process(Samples start, Samples sampleCount, float* pData, int channelCount);
		private:
			FileData* mpFile;
			Samples mReadPointer;
			float mReadFraction;
			int mRepeatAmount;
			float mPlayRate;
		};

		class Gain : public Plugin
		{
		public:
			Gain(Plugin* pNext = nullptr, float gain = 1.0f);
			void SetGain(float gain);
			void FadeToFull(unsigned long sampleDuration);
			void FadeToEmpty(unsigned long sampleDuration);
			virtual Status Process(Samples start, Samples sampleCount, float* pData, int channelCount);
		private:
			float mGain;
			float mGainSpeed;
		};
	}
}