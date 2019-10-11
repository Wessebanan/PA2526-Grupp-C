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
			}
			// Once one plugin gets deleted, the chain of
			// plugins gets deleted
			~Plugin() {
				if (mpNext != nullptr)
				{
					delete mpNext;
					mpNext = nullptr;
				}
			}
			virtual Status Process(Samples start, Samples sampleCount, float* pData, int channelCount) = 0;
		protected:
			Plugin* mpNext;
		};

		class Sampler : public Plugin
		{
		public:
			Sampler(File::SoundData* pFile, int repeatAmount);
			virtual Status Process(Samples start, Samples sampleCount, float* pData, int channelCount);
		private:
			File::SoundData* mpFile;
			Samples mReadPointer;
			int mRepeatAmount;
		};
	}
}