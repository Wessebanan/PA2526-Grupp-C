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
		"cc_song.wav",
		"cc_drums.wav"
	};
	ASSERT_TRUE(bank.LoadMultipleFiles(file_paths, 2));

	// Start engine
	SetupEngine(engine, mixer, pa_init);

	std::cout << "Add music..." << std::endl;
	Audio::FileData* hello = bank[0];
	mixer.AddMusicMessage({
		bank[0],
		Audio::Music::M_FUNC_REPLACE_MUSIC |
		Audio::Music::M_DATA_AS_PARAMETER
		});
	mixer.AddMusicMessage({
		bank[1],
		Audio::Music::M_FUNC_REPLACE_MUSIC |
		Audio::Music::M_TARGET_SUB |
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_SYNC_OTHER_WITH_THIS
		});
	Pa_Sleep(1000);
	
	float speed;
	std::cout << "SPEED UP!!!" << std::endl;
	for (speed = 1.0f; speed < 1.1f; speed += 0.005f)
	{
		mixer.AddMusicMessage({
			speed,
			Audio::Music::M_FUNC_SET_SPEED |
			Audio::Music::M_DATA_AS_PARAMETER
			});
		Pa_Sleep(100);
	}
	
	mixer.AddMusicMessage({
		0.0f,
		Audio::Music::M_FUNC_SET_GAIN |
		Audio::Music::M_TARGET_SUB |
		Audio::Music::M_DATA_AS_PARAMETER
		});
	std::cout << "Mute drums" << std::endl;
	for (; speed < 1.2f; speed += 0.005f)
	{
		mixer.AddMusicMessage({
			speed,
			Audio::Music::M_FUNC_SET_SPEED |
			Audio::Music::M_DATA_AS_PARAMETER
			});
		Pa_Sleep(100);
	}

	mixer.AddMusicMessage({
	1.0f,
	Audio::Music::M_FUNC_SET_GAIN |
	Audio::Music::M_TARGET_SUB |
	Audio::Music::M_DATA_AS_PARAMETER
		});
	std::cout << "Unmute drums" << std::endl;
	for (; speed < 1.3f; speed += 0.005f)
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

TEST(SoundAPI, FourierTransform)
{
	// Initialize a Audio engine, mixer and bank
	Audio::Engine engine;
	Audio::Mixer mixer;
	Audio::Bank bank;
	Audio::PaHandler pa_init;

	// Read song into bank
	std::string file_paths[] =
	{
		"cc_song.wav",
		"cc_drums.wav"
	};
	ASSERT_TRUE(bank.LoadMultipleFiles(file_paths, 2));

	// Start engine
	SetupEngine(engine, mixer, pa_init);

	std::cout << "Add music..." << std::endl;
	Audio::FileData* hello = bank[0];
	mixer.AddMusicMessage({
		bank[0],
		Audio::Music::M_FUNC_REPLACE_MUSIC |
		Audio::Music::M_DATA_AS_PARAMETER
		});
	mixer.AddMusicMessage({
		bank[1],
		Audio::Music::M_FUNC_REPLACE_MUSIC |
		Audio::Music::M_TARGET_SUB |
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_SYNC_OTHER_WITH_THIS
		});
	Pa_Sleep(1000);
	float freq_array[256];
	for (int i = 0; i < 50; i++)
	{
		mixer.GetMusicManager()->GetFrequencies(freq_array);
		std::cout << freq_array[4] << std::endl;
		Pa_Sleep(100);
	}

	TeardownEngine(engine);

	std::cout << "End of test\n";
}