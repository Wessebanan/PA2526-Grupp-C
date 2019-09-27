#include "SoundEngine.h"
#include <iostream>

Sound::Engine::Engine() : mpStream(0), mConsumeBufferIndex(0)
{
	// Initialize chain buffers
	for (int i = 0; i < SOUND_CHAIN_BUFFER_COUNT; i++)
	{
		mpChainBuffers[i] = new Buffer(2, SOUND_FRAMES_PER_BUFFER);
	}
}

bool Sound::Engine::OpenStream()
{
	return OpenStream(Pa_GetDefaultOutputDevice());
}

bool Sound::Engine::OpenStream(PaDeviceIndex index)
{
	PaStreamParameters output_parameters;

	// Double check that the device is valid
	output_parameters.device = index;
	if (output_parameters.device == paNoDevice) {
		return false;
	}

	output_parameters.channelCount = 2;			// Stereo output 
	output_parameters.sampleFormat = paFloat32;	// 32 bit floating point output
	output_parameters.suggestedLatency =			// Why wouldn't we want lowest latency?
		Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
	output_parameters.hostApiSpecificStreamInfo = NULL;	// Won't use this

	PaError err = Pa_OpenStream(
		&mpStream,
		NULL,						// We won't be using any input (Microphones etc)
		&output_parameters,
		SOUND_SAMPLE_RATE,			// Sample rate of 44100 hz (standard)
		64,
		paClipOff,		// we won't output out of range samples so don't bother clipping them
		&Sound::Engine::PaCallback,
		this            // Using 'this' for userData so we can cast to Engine* in paCallback method
	);

	if (err != paNoError)
	{
		/* Failed to open stream to device !!! */
		return false;
	}

	err = Pa_SetStreamFinishedCallback(mpStream, &Sound::Engine::PaStreamFinished);

	// In case an error occures, close the stream
	if (err != paNoError)
	{
		Pa_CloseStream(mpStream);
		mpStream = nullptr;

		return false;
	}

	return true;
}

bool Sound::Engine::CloseStream()
{
	if (mpStream == 0)
		return false;

	PaError err = Pa_CloseStream(mpStream);
	mpStream = 0;

	return (err == paNoError);
}

bool Sound::Engine::StartStream()
{
	if (mpStream == 0)
		return false;

	PaError err = Pa_StartStream(mpStream);

	return (err == paNoError);
}

bool Sound::Engine::StopStream()
{
	if (mpStream == 0)
		return false;

	PaError err = Pa_StopStream(mpStream);

	return (err == paNoError);
}

Sound::Buffer* Sound::Engine::__GetChainBuffer(int index)
{
	return mpChainBuffers[index];
}

int Sound::Engine::PaCallbackMethod(const void* pInputBuffer, void* pOutputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* pTimeInfo,
	PaStreamCallbackFlags statusFlags)
{
	float* out = (float*)pOutputBuffer;
	unsigned long i;

	(void)pTimeInfo;				// Are not used right now, this is to prevent
	(void)statusFlags;			// the unused warning
	(void)pInputBuffer;

	for (i = 0; i < framesPerBuffer; i++)
	{
		*out++ = mpChainBuffers[mConsumeBufferIndex]->Data[0][i];  /* left */
		*out++ = mpChainBuffers[mConsumeBufferIndex]->Data[1][i];  /* right */
	}

	return paContinue;

}

// This routine will be called by the PortAudio engine when audio is needed.
// It may called at interrupt level on some machines so don't do anything
// that could mess up the system like calling malloc() or free().
// 
int Sound::Engine::PaCallback(const void* pInputBuffer, void* pOutputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* pTimeInfo,
	PaStreamCallbackFlags statusFlags,
	void* pUserData)
{
// Here we cast userData to Engine* type so we can call the instance method paCallbackMethod,
// we can do that since we called Pa_OpenStream with 'this' for userData
	return ((Engine*)pUserData)->PaCallbackMethod(pInputBuffer, pOutputBuffer,
		framesPerBuffer,
		pTimeInfo,
		statusFlags);
}


void Sound::Engine::PaStreamFinishedMethod()
{
	// Not being used right now
}

/*
 * This routine is called by portaudio when playback is done.
 */
void Sound::Engine::PaStreamFinished(void* pUserData)
{
	return ((Engine*)pUserData)->PaStreamFinishedMethod();
}