#include "pch.h"
#include "SoundEngineTester.h"
#include "RingBuffer.h"
#include <thread>
#include <cmath>

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

// MOST TEST REQUIRES HEADPHONES OR LOUDSPEAKER TO BE PLUGGED IN!!!

// Test based on the task:
// "Setup PortAudio to stream from buffers"
// Definition of Done:
// "Should be able to play a sine wave that is stored
// in a temporary play buffer"
TEST(SoundAPI, InitializePortAudio)
{
	// Initialize a TestSoundEngine (Extended Sound Engine)
	TestSpace::Sound::Engine engine;

	// Reach the first chain buffer
	// TODO LATER: Once the chain buffer works as intended
	// and shifts which buffer to consume, we can't assume
	// that the engine only consumes chain buffer 0.
	// Needs to be edited once that's implemented!
	Sound::Buffer* chain_buffer = engine.GetChainBuffer(0);

	// Fill both channels of the buffer with one period of
	// a sine wave
	for (int i = 0; i < SOUND_FRAMES_PER_BUFFER; i++)
	{
		chain_buffer->Data[0][i] =
			(float)sin(((double)i / (double)SOUND_FRAMES_PER_BUFFER) * M_PI * 2.);
		chain_buffer->Data[1][i] =
			(float)sin(((double)i / (double)SOUND_FRAMES_PER_BUFFER) * M_PI * 2.);
	}

	std::cout << "PortAudio Test: output sine wave. SR = " << SOUND_SAMPLE_RATE
		<< ", BufSize = " << SOUND_FRAMES_PER_BUFFER << std::endl;

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

	std::cout << "Play for 2 seconds.\n";
	Pa_Sleep(2 * 1000);

	// Tear-down
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