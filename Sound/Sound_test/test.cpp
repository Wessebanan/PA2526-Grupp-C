#include "pch.h"

#include "SoundEngine.h"
#include "RingBuffer.h"
#include "TestPlugins.h"
#include "SoundBank.h"
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
	// Initialize a Sound Engine
	Sound::Engine engine;
	Sound::Mixer mixer;

	// Add voices to the mixer
	{
		using namespace Sound::Plugin;

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
	// Have the engine use this mixer
	engine.UseThisMixer(&mixer);

	std::cout << "PortAudio Test: output sine wave. SR = " << SOUND_SAMPLE_RATE
		<< ", RingBufSize = " << SOUND_BUFFER_SIZE << std::endl;

	// Instance PortAudio handler
	Sound::PaHandler pa_init;

	ASSERT_EQ(pa_init.result(), paNoError)
		<< "An error occured while using the portaudio stream"
		<< "\nError number: " << pa_init.result()
		<< "\nError message: " << Pa_GetErrorText(pa_init.result());

	// Open stream
	ASSERT_TRUE(engine.OpenStream());
	// Start feeding the sound card using the callback function
	ASSERT_TRUE(engine.StartStream());

	engine.StartWorkThread();	// Start the work thread
								// to fill the ringbuffer

	std::cout << "Play for 2 seconds.\n";
	Pa_Sleep(2 * 1000);

	// Tear-down
	engine.JoinWorkThread();	// End the work thread

	engine.StopStream();

	engine.CloseStream();

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
	Sound::Bank bank;
	// Loading a non existing file should result in a nullptr (0)
	EXPECT_FALSE(bank.GetFile("non_existing_file"));
	// Check if sine.wav only has one channel
	Sound::FileData* p_sine_file = bank.GetFile("sine.wav");
	EXPECT_EQ(p_sine_file->GetNumChannels(), 1);
	// Check if sine2.wav has two channels
	Sound::FileData* p_sine_file2 = bank.GetFile("sine2.wav");
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
	// Initialize a Sound engine, mixer and bank
	Sound::Engine engine;
	Sound::Mixer mixer;
	Sound::Bank bank;

	// Add voice to the mixer
	{
		using namespace Sound::Plugin;

		// Load "square.wav"
		Sound::FileData* p_file = bank.GetFile("square.wav");
		EXPECT_NE(p_file, nullptr);

		std::cout << "The file is "
			<< ((float)p_file->GetFrameCount() / (float)SOUND_SAMPLE_RATE)
			<< " seconds long\n";

		mixer.AddSoundMessage({ new Sampler(p_file) });

	}
	// Have the engine use this mixer
	engine.UseThisMixer(&mixer);

	std::cout << "PortAudio Test: output sine wave. SR = " << SOUND_SAMPLE_RATE
		<< ", RingBufSize = " << SOUND_BUFFER_SIZE << std::endl;

	// Instance PortAudio handler
	Sound::PaHandler pa_init;

	ASSERT_EQ(pa_init.result(), paNoError)
		<< "An error occured while using the portaudio stream"
		<< "\nError number: " << pa_init.result()
		<< "\nError message: " << Pa_GetErrorText(pa_init.result());

	// Open stream
	ASSERT_TRUE(engine.OpenStream());
	// Start feeding the sound card using the callback function
	ASSERT_TRUE(engine.StartStream());

	engine.StartWorkThread();	// Start the work thread
								// to fill the ringbuffer

	std::cout << "Play for 2.5 seconds.\n";
	Pa_Sleep(2500);

	// Tear-down
	engine.JoinWorkThread();	// End the work thread

	engine.StopStream();

	engine.CloseStream();

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
	Sound::Bank bank;
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
	Sound::Bank bank_2;
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
	Sound::Bank bank_3;
	// Should fail
	EXPECT_FALSE(bank_3.LoadMultipleFiles(FILE_NAMES_3, 27));
}