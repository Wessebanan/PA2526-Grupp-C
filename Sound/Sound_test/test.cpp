#include "pch.h"
#include "SoundEngineTester.h"

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