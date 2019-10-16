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
			Sampler(FileData* pFile, int repeatAmount);
			Sampler();
			void SetFileAndReset(FileData* pFile);
			Samples GetReadPointer();
			void SetReadPointer(Samples readPointer);
			virtual Status Process(Samples start, Samples sampleCount, float* pData, int channelCount);
		private:
			FileData* mpFile;
			Samples mReadPointer;
			int mRepeatAmount;
		};
	}
}