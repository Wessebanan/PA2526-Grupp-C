#pragma once
#include "SoundHelpers.h"

namespace Sound
{
	class Plugin
	{
	public:
		virtual void Process(Samples Start, Samples Count, int Channels, float* Frames[2]) = 0;
	};

	class TestSineWave : public Plugin
	{
	public:
		TestSineWave() {};
		void Process(Samples Start, Samples Count, int Channels, float* Frames[2]);
	};
}