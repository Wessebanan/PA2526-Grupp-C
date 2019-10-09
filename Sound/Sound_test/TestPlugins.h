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
			Status Process(Samples start, Samples sampleCount, float* pData, int channelCount);
		private:
			float mFrequency;
		};

		// Does nothing and only calls the next plugin in the line
		class Passthrough : public Plugin
		{
		public:
			Passthrough(Plugin* pNext);
			Status Process(Samples start, Samples sampleCount, float* pData, int channelCount);
		};
	}
}