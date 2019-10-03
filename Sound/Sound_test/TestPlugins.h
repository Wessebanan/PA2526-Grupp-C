#pragma once
#include "Plugin.h"

namespace Sound
{
	namespace Plugin
	{
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