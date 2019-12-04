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

TEST(SoundAPI, MusicSpeedUp)
{
	// Initialize a Audio engine, mixer and bank
	Audio::Engine engine;
	Audio::Mixer mixer;
	Audio::Bank bank;
	Audio::PaHandler pa_init;

	// Read song into bank
	std::string file_paths[] =
	{
		"cc_song.wav"//,
		//"cc_drums.wav"
	};
	ASSERT_TRUE(bank.LoadMultipleFiles(file_paths, 1));

	// Start engine
	SetupEngine(engine, mixer, pa_init);

	std::cout << "Add music..." << std::endl;
	Audio::FileData* hello = bank[0];
	mixer.AddMusicMessage({
		bank[0],
		Audio::Music::M_FUNC_REPLACE_MUSIC |
		Audio::Music::M_DATA_AS_PARAMETER
		});
	Pa_Sleep(1000);
	
	std::cout << "SPEED UP!!!" << std::endl;
	for (float speed = 1.0f; speed < 1.3f; speed += 0.005f)
	{
		mixer.AddMusicMessage({
			speed,
			Audio::Music::M_FUNC_SET_SPEED |
			Audio::Music::M_DATA_AS_PARAMETER
			});
		Pa_Sleep(100);
	}

	TeardownEngine(engine);

	std::cout << "End of test\n";
}