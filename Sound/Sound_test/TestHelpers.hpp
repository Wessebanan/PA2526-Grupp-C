#pragma once
#include "pch.h"
#include "AudioEngine.h"
#include "RingBuffer.h"
#include "TestPlugins.h"
#include "AudioBank.h"

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

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