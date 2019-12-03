#pragma once
#include "TestHelpers.hpp"


TEST(SoundAPI, PlayDifferentPitches)
{
	// Random seed
	srand(time(NULL));
	// Initialize a Audio engine, mixer and bank
	Audio::Engine engine;
	Audio::Mixer mixer;
	Audio::Bank bank;
	Audio::PaHandler pa_init;

	// Add voice to the mixer
	using namespace Audio::Plugin;

	// Load "square.wav"
	Audio::FileData* p_file = bank.GetFile("coin.wav");
	ASSERT_NE(p_file, nullptr);

	// Setup
	SetupEngine(engine, mixer, pa_init);

	std::cout << "Play for 2.5 seconds.\n";
	for (int i = 0; i < 15; i++)
	{
		// Add coin sounds of different pitches for 1 1/2 sec
		mixer.AddSoundMessage({ new Sampler(p_file, 0, (rand() % 1000) / 2000.f + 0.5f) });
		Pa_Sleep(100);
	}

	// Tear-down
	TeardownEngine(engine);

	std::cout << "Test finished.\n";
}