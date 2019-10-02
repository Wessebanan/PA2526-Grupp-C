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
			virtual void Process(Samples Start, Samples Count, float* Data, int Channels) = 0;
		};

		// Generates a test sine wave and fills two channels
		class TestSineWave : public Plugin
		{
		public:
			TestSineWave() {};
			void Process(Samples Start, Samples Count, float* Data, int Channels);
		};

	}
}