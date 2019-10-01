#include "SoundEngine.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Sound::Engine::Engine()
{
	mpStream = nullptr;
	mWorkerThreadRun = false;
	mpWorkerThread = nullptr;
	mProducerLastSampleCount = 0;
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
		SOUND_FRAMES_PER_BUFFER,
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

void Sound::Engine::StartWorkThread()
{
	mWorkerThreadRun = true;
	mWorkThreadStartTime = std::chrono::steady_clock::now();
	mpWorkerThread = new std::thread(&WorkerThreadUpdate, this);
}

void Sound::Engine::JoinWorkThread()
{
	mWorkerThreadRun = false;
	mpWorkerThread->join();
}

int Sound::Engine::PaCallbackMethod(const void* pInputBuffer, void* pOutputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* pTimeInfo,
	PaStreamCallbackFlags statusFlags)
{
	float* out = (float*)pOutputBuffer;
	unsigned long i;

	(void)pTimeInfo;			// Are not used right now, this is to prevent
	(void)statusFlags;			// the unused warning
	(void)pInputBuffer;

	std::pair<float, float> temp_pair[SOUND_FRAMES_PER_BUFFER];

	unsigned long available = (unsigned long)mBuffer.readAvailable();

	unsigned long consume_count = std::min(framesPerBuffer, available);

	mBuffer.readBuff(temp_pair, consume_count);

	for (i = 0; i < consume_count; i++)
	{
		*out++ = temp_pair[i].first;  /* left */
		*out++ = temp_pair[i].second;  /* right */
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

void Sound::Engine::WorkerThreadUpdateMethod()
{
	// Get current sample count
	Samples current_sample_count = GetWorkerCurrentSampleCount();
	// Check how many frames should be processed
	// until the buffer have caught up
	Samples sample_count_since_last =
		current_sample_count - mProducerLastSampleCount;
	// Check available space in the buffer
	size_t available_buffer_space = mBuffer.writeAvailable();
	// Only fill what will fit, capped to SOUND_FRAMES_PER_BUFFER
	Samples samples_to_fill = std::min((Samples)SOUND_FRAMES_PER_BUFFER,
		std::min(sample_count_since_last, available_buffer_space));
	//if (available_buffer_space == 0)
	//	std::cout << "Overfilled ";

	// TEMPORARY -----------------------------------------
	// This will be removed once it's no longer needed for
	// testing purposes. Produces a sine wave to fill the
	// ring buffer with
	float temp_float_array[SOUND_FRAMES_PER_BUFFER];
	_FillWithSinus(mProducerLastSampleCount, samples_to_fill, temp_float_array);

	// Take the signal and fill it on both channels
	std::pair<float, float> temp_pair[SOUND_FRAMES_PER_BUFFER];
	for (Samples i = 0; i < samples_to_fill; i++)
	{
		temp_pair[i].first = temp_float_array[i];
		temp_pair[i].second = temp_float_array[i];
	}
	// TEMPORARY END -------------------------------------

	// Write to the ring buffer
	mBuffer.writeBuff(temp_pair, samples_to_fill);

	// Progress the last sample count by the amount
	// of samples filled this iteration
	mProducerLastSampleCount += samples_to_fill;
}

void Sound::Engine::WorkerThreadUpdate(void* data)
{
	while (((Engine*)data)->mWorkerThreadRun)
	{
		// SPEEEN (Temporary spin loop implementation)
		((Engine*)data)->WorkerThreadUpdateMethod();
	}
}

inline Samples Sound::Engine::GetWorkerCurrentSampleCount()
{
	// To get the desired precision, the duration is casted
	// to nanoseconds and then converted back to seconds
	float temp = std::chrono::duration_cast<std::chrono::nanoseconds>
		(std::chrono::steady_clock::now() - mWorkThreadStartTime).count() * 0.000000001f;
	return ToSamples(temp);
}

inline Samples Sound::Engine::ToSamples(const float Seconds)
{
	return (Samples)(Seconds * (float)SOUND_SAMPLE_RATE);
}

inline float Sound::Engine::ToSeconds(const Samples SampleCount)
{
	return (float)SampleCount / (float)SOUND_SAMPLE_RATE;
}

void Sound::Engine::_FillWithSinus(Samples CurrSample, Samples SampleCount, float* Buffer)
{
	const float FREQUENCY = 440.0f;	// Frequency of the wave
	const float AMPLITUDE = 0.2f;	// To save people's ears; Low amplitude
	for (Samples i = 0; i < SampleCount; i++)
	{
		Buffer[i] = std::sinf(FREQUENCY * ToSeconds(CurrSample + i) * 2.0f * (float)M_PI) * AMPLITUDE;
	}
}
