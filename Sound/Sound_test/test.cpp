#include "pch.h"

#include "AudioEngine.h"
#include "RingBuffer.h"
#include "TestPlugins.h"
#include "AudioBank.h"
#include <thread>
#include <cmath>

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#include <crtdbg.h>
int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

// To reduce code in tests, the engine setupa and teardown
// have moved to separate functions
inline
void SetupEngine(Audio::Engine& rEngine, Audio::Mixer& rMixer, Audio::PaHandler& rPaHandler)
{
	// Have the engine use this mixer
	rEngine.UseThisMixer(&rMixer);

	std::cout << "PortAudio Test: output sine wave. SR = " << SOUND_SAMPLE_RATE
		<< ", RingBufSize = " << SOUND_BUFFER_SIZE << std::endl;

	// Instance PortAudio handler
	

	ASSERT_EQ(rPaHandler.result(), paNoError)
		<< "An error occured while using the portaudio stream"
		<< "\nError number: " << rPaHandler.result()
		<< "\nError message: " << Pa_GetErrorText(rPaHandler.result());

	// Open stream
	ASSERT_TRUE(rEngine.OpenStream());
	// Start feeding the sound card using the callback function
	ASSERT_TRUE(rEngine.StartStream());

	rEngine.StartWorkThread();	// Start the work thread
								// to fill the ringbuffer
}

inline
void TeardownEngine(Audio::Engine& rEngine)
{
	rEngine.JoinWorkThread();	// End the work thread

	rEngine.StopStream();

	rEngine.CloseStream();
}

// MOST TEST REQUIRES HEADPHONES OR LOUDSPEAKER TO BE PLUGGED IN!!!

// Test based on the task:
// "Setup PortAudio to stream from buffers"
// Definition of Done:
// "Should be able to play a sine wave that is stored
// in a temporary play buffer"
// ... And the task:
// "Setup separate thread for sound engine loop"
// Definition of Done:
// "The thread fills the ring buffer when needed"
TEST(SoundAPI, InitializePortAudio)
{
	// Initialize a Audio Engine
	Audio::Engine engine;
	Audio::Mixer mixer;
	Audio::PaHandler pa_init;

	// Add voices to the mixer
	{
		using namespace Audio::Plugin;

		// Test so that chained plugins doesn't cause trouble
		mixer.AddSoundMessage(
			{
			new Passthrough(
				new Passthrough(
					new Passthrough(
						new Passthrough(
							new Passthrough(
								new TestSineWave(440.f)
			)))))
			});
		mixer.AddSoundMessage({ new TestSineWave(220.f) });
		mixer.AddSoundMessage({ new TestSineWave(110.f) });

	}

	SetupEngine(engine, mixer, pa_init);

	std::cout << "Play for 2 seconds.\n";
	Pa_Sleep(2 * 1000);

	// Tear-down
	TeardownEngine(engine);

	std::cout << "Test finished.\n";
}

// Tests based on the task:
// "Create a circular buffer"
// Definition of Done:
// "The buffer should be able to write and consume data
// locklessly between threads"

TEST(Utility, RingBuffer)
{
	const int SIZE = 512;	// Has to be the power of 2
	Ringbuffer<float, SIZE> buffer;	// Create a ring buffer
	float temp_float = 2.0f;
	buffer.insert(&temp_float);		// Insert 2.0f
	temp_float = 0.0f;
	// Read one float from the buffer, expect to have read one float
	EXPECT_EQ(buffer.readBuff(&temp_float, 1), 1);
	// It should be 2.0f
	EXPECT_EQ(temp_float, 2.0f);

	// Create a float array and fill with incrementing floats
	float temp_float_array[SIZE*2];
	for (int i = 0; i < SIZE*2; i++)
	{
		temp_float_array[i] = (float)i;
	}
	// Check if all space is available
	EXPECT_TRUE(buffer.isEmpty());
	EXPECT_EQ(buffer.writeAvailable(), SIZE);
	// Fill the entire buffer and check if it's full
	EXPECT_EQ(buffer.writeBuff(temp_float_array, SIZE), SIZE);
	EXPECT_EQ(buffer.writeAvailable(), 0);
	EXPECT_TRUE(buffer.isFull());

	// Check so that the buffer contains the right data
	for (int i = 0; i < SIZE; i++)
	{
		buffer.readBuff(&temp_float, 1);
		EXPECT_EQ(temp_float, (float)i);
	}
	EXPECT_TRUE(buffer.isEmpty());	// ... and is empty now

	// Try writing more than the buffer can store
	EXPECT_EQ(buffer.writeAvailable(), SIZE);
	EXPECT_EQ(buffer.writeBuff(temp_float_array, SIZE*2), SIZE);
	EXPECT_EQ(buffer.writeAvailable(), 0);
	EXPECT_TRUE(buffer.isFull());

	// Check so that the buffer contains the right data
	for (int i = 0; i < SIZE; i++)
	{
		buffer.readBuff(&temp_float, 1);
		EXPECT_EQ(temp_float, (float)i);
	}
	// Reading more should not work
	for (int i = 0; i < SIZE; i++)
	{
		EXPECT_EQ(buffer.readBuff(&temp_float, 1), 0);
	}
	EXPECT_TRUE(buffer.isEmpty());	// Should be empty

}

// The non-main thread function to read data from main thread
void WorkingThreadFunction(void* data)
{
	// Get buffer reference from main thread
	Ringbuffer<float, 64>& buffer =
		*((Ringbuffer<float, 64>*)data);

	float temp_float_array[40];
	int number = 0;
	int count;
	for (int i = 0; i < 100; i++)	// Do 100 iterations
	{
		// Read a random amount of times, but not more than possible
		count = rand() % 20 + 20;
		count = buffer.readBuff(temp_float_array, count);
		if (buffer.isEmpty())
			std::cout << "E";	// Prints E if empty
		for (int j = 0; j < count; j++)
		{
			// We expect that the data read from the buffer
			// is an incrementing float with no skips
			// (which would probably mean data was overwritten)
			EXPECT_EQ(temp_float_array[j], (float)number++);
		}
		// To give this thread a chance to work alongside 
		// the main thread, put it to sleep for approx. 30ms
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
}


TEST(Utility, RingBufferThreaded)
{
	std::cout
		<< "For every E, the ring buffer tried to be read from, but was empty\n"
		<< "For every F, the ring buffer tried to be written to, but was full\n"
		<< "This behaviour is expected since one thread could be scheduled "
		<< "differently from the other:\n";

	Ringbuffer<float, 64> buffer;	// Create ring buffer
	std::thread work_thread(&WorkingThreadFunction, &buffer);	// Start work thread

	float temp_float_array[40];
	int number = 0;
	int count;
	for (int i = 0; i < 100; i++)	// Do 100 iterations
	{
		// Write arandom amount of times, but not more than possible
		count = rand() % 20 + 20;
		count = std::min((int)buffer.writeAvailable(), count);
		if (buffer.isFull())
			std::cout << "F";	// Prints F when full
		for (int j = 0; j < count; j++)
		{
			// Write continuously incrementing floats
			temp_float_array[j] = (float)number++;
		}
		buffer.writeBuff(temp_float_array, count);	// write to buffer
		// To give this thread a chance to work alongside 
		// the worker thread, put it to sleep for approx. 30ms
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
	work_thread.join();	// wait for worker thread to finish
	std::cout << "\nTotal floats processed: " << number << std::endl;
}


// Tests based on the task:
// "Load sound data"
// Definition of Done:
// "A number of WAV files will be read from disk and placed
// in memory"
TEST(SoundAPI, LoadSoundData)
{
	// Initialize a new sound bank
	Audio::Bank bank;
	// Loading a non existing file should result in a nullptr (0)
	EXPECT_FALSE(bank.GetFile("non_existing_file"));
	// Check if sine.wav only has one channel
	Audio::FileData* p_sine_file = bank.GetFile("sine.wav");
	EXPECT_EQ(p_sine_file->GetNumChannels(), 1);
	// Check if sine2.wav has two channels
	Audio::FileData* p_sine_file2 = bank.GetFile("sine2.wav");
	EXPECT_EQ(p_sine_file2->GetNumChannels(), 2);
}

// Tests based on the task:
// "Create sampler plugin"
// Definition of Done:
// "The sampler will read the correct amount of sound data
// and forward it (To test: Play a looped sound from the
// sound data)"
TEST(SoundAPI, PlaySoundWithSampler)
{
	// Initialize a Audio engine, mixer and bank
	Audio::Engine engine;
	Audio::Mixer mixer;
	Audio::Bank bank;
	Audio::PaHandler pa_init;

	// Add voice to the mixer
	{
		using namespace Audio::Plugin;

		// Load "square.wav"
		Audio::FileData* p_file = bank.GetFile("square.wav");
		EXPECT_NE(p_file, nullptr);

		std::cout << "The file is "
			<< ((float)p_file->GetFrameCount() / (float)SOUND_SAMPLE_RATE)
			<< " seconds long\n";

		mixer.AddSoundMessage({ new Sampler(p_file, 0) });

	}
	
	SetupEngine(engine, mixer, pa_init);

	std::cout << "Play for 2.5 seconds.\n";
	Pa_Sleep(2500);

	// Tear-down
	TeardownEngine(engine);

	std::cout << "Test finished.\n";
}

TEST(SoundAPI, SoundBankReadMany)
{
	// Load three sound files that does exist
	const std::string FILE_NAMES[] =
	{
		"sine.wav",
		"sine2.wav",
		"square.wav"
	};
	Audio::Bank bank;
	// Should be successful
	EXPECT_TRUE(bank.LoadMultipleFiles(FILE_NAMES, 3));

	// Bank should have the three filedatas
	for (int i = 0; i < 3; i++)
	{
		EXPECT_NE(bank[i], nullptr);
	}
	// This is nonsense and should return nullptr
	EXPECT_EQ(bank[100000], nullptr);

	// Load four, which one does not exist
	const std::string FILE_NAMES_2[] =
	{
		"sine.wav",
		"thisdoesnotexist.wav",
		"sine2.wav",
		"square.wav"
	};
	Audio::Bank bank_2;
	// Should fail
	EXPECT_FALSE(bank_2.LoadMultipleFiles(FILE_NAMES_2, 4));
	// The sound file that does not exist should be nullptr
	EXPECT_EQ(bank_2[1], nullptr);

	// Read too many sound files into the bank
	const std::string FILE_NAMES_3[] =
	{
		"sine.wav","sine2.wav","square.wav",
		"sine.wav","sine2.wav","square.wav",
		"sine.wav","sine2.wav","square.wav",
		"sine.wav","sine2.wav","square.wav",
		"sine.wav","sine2.wav","square.wav",
		"sine.wav","sine2.wav","square.wav",
		"sine.wav","sine2.wav","square.wav",
		"sine.wav","sine2.wav","square.wav",
		"sine.wav","sine2.wav","square.wav"
	};
	Audio::Bank bank_3;
	// Should fail
	EXPECT_FALSE(bank_3.LoadMultipleFiles(FILE_NAMES_3, 27));
}


TEST(SoundAPI, MusicThroughMessaging)
{
	// Initialize a Audio engine, mixer and bank
	Audio::Engine engine;
	Audio::Mixer mixer;
	Audio::Bank bank;
	Audio::PaHandler pa_init;

	// Read song into bank
	std::string file_paths[] =
	{
		"cc_song.wav"
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

	std::cout << "Attempt to add music WITH the replace flag..." << std::endl;
	mixer.AddMusicMessage({
		bank[0],
		Audio::Music::M_FUNC_REPLACE_MUSIC |
		Audio::Music::M_DATA_AS_PARAMETER
		});
	Pa_Sleep(1000);

	TeardownEngine(engine);

	std::cout << "End of test\n";
}

TEST(SoundAPI, MusicAndSoundMessaging)
{
	// Initialize a Audio engine, mixer and bank
	Audio::Engine engine;
	Audio::Mixer mixer;
	Audio::Bank bank;
	Audio::PaHandler pa_init;

	// Read both music and sound effect
	std::string file_paths[] =
	{
		"cc_song.wav",
		"coin.wav"
	};
	ASSERT_TRUE(bank.LoadMultipleFiles(file_paths, 2));

	// Setup engine
	SetupEngine(engine, mixer, pa_init);

	std::cout << "Adds music every second (shouldn't interrupt song)" << std::endl;
	std::cout << "Adds sound 20 times a second" << std::endl;
	for (int i = 0; i < 3; i++)
	{
		mixer.AddMusicMessage({
			bank[0],
			Audio::Music::M_FUNC_REPLACE_MUSIC |
			Audio::Music::M_DATA_AS_PARAMETER
			});
		for (int j = 0; j < 20; j++)
		{
			mixer.AddSoundMessage({
				new Audio::Plugin::Sampler(bank[1],1)
				});
			Pa_Sleep(50);	
		}

	}

	TeardownEngine(engine);

	std::cout << "End of test\n";
}

TEST(SoundAPI, MusicMixing)
{
	// Initialize a Audio engine, mixer and bank
	Audio::Engine engine;
	Audio::Mixer mixer;
	Audio::Bank bank;
	Audio::PaHandler pa_init;

	// Read both music and sound effect
	std::string file_paths[] =
	{
		"cc_song.wav",
		"cc_drums.wav"
	};
	ASSERT_TRUE(bank.LoadMultipleFiles(file_paths, 2));

	// Setup engine
	SetupEngine(engine, mixer, pa_init);

	mixer.AddMusicMessage({
		bank[0],
		Audio::Music::M_FUNC_REPLACE_MUSIC |
		Audio::Music::M_DATA_AS_PARAMETER
	});
	mixer.AddMusicMessage({
		0.0f,
		Audio::Music::M_FUNC_SET_GAIN |
		Audio::Music::M_DATA_AS_PARAMETER
		});
	mixer.AddMusicMessage({
		(unsigned long)80000,
		Audio::Music::M_FUNC_FADE_IN |
		Audio::Music::M_DATA_AS_PARAMETER
	});
	Pa_Sleep(4000);

	mixer.AddMusicMessage({
		bank[1],
		Audio::Music::M_FUNC_REPLACE_MUSIC |
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_TARGET_SUB |
		Audio::Music::M_SYNC_THIS_WITH_OTHER
	});

	mixer.AddMusicMessage({
		0.0f,
		Audio::Music::M_FUNC_SET_GAIN |
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_TARGET_SUB
		});

	mixer.AddMusicMessage({
		(unsigned long)80000,
		Audio::Music::M_FUNC_FADE_IN |
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_TARGET_SUB
	});

	Pa_Sleep(4000);

	mixer.AddMusicMessage({
		(unsigned long)120000,
		Audio::Music::M_FUNC_FADE_OUT |
		Audio::Music::M_DATA_AS_PARAMETER
		});

	mixer.AddMusicMessage({
		(unsigned long)120000,
		Audio::Music::M_FUNC_FADE_OUT |
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_TARGET_SUB
		});

	Pa_Sleep(3000);

	TeardownEngine(engine);

	std::cout << "End of test\n";
}