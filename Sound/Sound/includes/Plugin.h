#pragma once
#include "SoundHelpers.h"

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
			virtual void Process(Samples Start, Samples Count, float* Data, int Channels) = 0;
		protected:
			Plugin* mpNext;
		};

		// Generates a test sine wave and fills two channels
		class TestSineWave : public Plugin
		{
		public:
			TestSineWave(float Frequency);
			void Process(Samples Start, Samples Count, float* Data, int Channels);
		private:
			float mFrequency;
		};

		// Does nothing and only calls the next plugin in the line
		class Passthrough : public Plugin
		{
		public:
			Passthrough(Plugin* pNext);
			void Process(Samples Start, Samples Count, float* Data, int Channels);
		};

	}
}