#pragma once
#include "SoundHelpers.h"
#include "SoundFile.h"

namespace Sound
{
	namespace Plugin
	{
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
			virtual void Process(Samples Start, Samples Count, float* pData, int Channels) = 0;
		protected:
			Plugin* mpNext;
		};

		class Sampler : public Plugin
		{
		public:
			Sampler(FileData* pFile);
			virtual void Process(Samples Start, Samples Count, float* pData, int Channels);
		private:
			FileData* mpFile;
			Samples mReadPointer;
		};
	}
}