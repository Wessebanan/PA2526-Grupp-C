#pragma once
#include "TestHelpers.hpp"

TEST(SoundAPI, ExtremeSoundEffectSpam)
{
	// Initialize a Audio engine, mixer and bank
	Audio::Engine engine;
	Audio::Mixer mixer;
	Audio::Bank bank;
	Audio::PaHandler pa_init;

	// Read sound effect
	std::string file_paths[] =
	{
		"coinlow.wav"
	};
	ASSERT_TRUE(bank.LoadMultipleFiles(file_paths, 1));

	// Setup engine
	SetupEngine(engine, mixer, pa_init);

	std::cout << "WARNING!!!\n*********LOWER THE VOLUME!!!\nLOUD NOISES!!!\n";
	Pa_Sleep(3000);
	for (int i = 0; i < 10000000; i++)
	{
		mixer.AddSoundMessage({
				new Audio::Plugin::Sampler(bank[0],1)
		});	
	}

	TeardownEngine(engine);

	std::cout << "End of test\n";
}