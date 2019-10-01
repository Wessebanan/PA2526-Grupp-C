#include "SoundEngine.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Sound::Engine::Engine()
{
	mpStream = nullptr;
	mWorkerThreadRun = false;
	mpWorkerThread = nullptr;
	mLastSampleCount = 0;
	//std::pair<float, float> temp_pair = { 0.0f, 0.0f };
	//for (int i = 0; i < SOUND_FRAMES_PER_BUFFER; i++)
	//{
	//	mBuffer.insert(&temp_pair);
	//}
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

Sound::Buffer* Sound::Engine::__GetChainBuffer(int index)
{
	//return mpChainBuffers[index];
	return nullptr;
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

	std::pair<float, float> temp_pair[512];

	unsigned long available = (unsigned long)mBuffer.readAvailable();
	//std::cout << available << " ";
	unsigned long consume_count = std::min(
		framesPerBuffer,
		available
	);
	//if (consume_count == 0)
	//{
	//	std::cout << "MISS";
	//	for (i = 0; i < framesPerBuffer; i++)
	//	{
	//		*out++ = 0.0f;
	//		*out++ = 0.0f;
	//	}
	//}
	//else
	//{
		mBuffer.readBuff(temp_pair, consume_count);

		for (i = 0; i < consume_count; i++)
		{
			*out++ = temp_pair[i].first;  /* left */
			*out++ = temp_pair[i].second;  /* right */
		}
	//}
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
	Samples current_sample_count = GetWorkerCurrentSampleCount();
	Samples sample_count_since_last =
		current_sample_count - mLastSampleCount;
	//std::cout << sample_count_since_last << " ";
	size_t available_buffer_space = mBuffer.writeAvailable();
	Samples samples_to_fill = std::min(sample_count_since_last, available_buffer_space);
	if (available_buffer_space == 0)
		std::cout << "N";
	//std::cout << samples_to_fill << " ";

	
	float temp_float_array[SOUND_BUFFER_SIZE];
	_FillWithSinus(mLastSampleCount, samples_to_fill, temp_float_array);

	std::pair<float, float> temp_pair[SOUND_BUFFER_SIZE];
	for (Samples i = 0; i < samples_to_fill; i++)
	{
		temp_pair[i].first = temp_float_array[i];
		temp_pair[i].second = temp_float_array[i];
	}
	mBuffer.writeBuff(temp_pair, samples_to_fill);

	mLastSampleCount = current_sample_count;
}

void Sound::Engine::WorkerThreadUpdate(void* data)
{
	while (((Engine*)data)->mWorkerThreadRun)
	{
		// SPEEEN
		((Engine*)data)->WorkerThreadUpdateMethod();
	}
}

inline Samples Sound::Engine::GetWorkerCurrentSampleCount()
{
	double temp = std::chrono::duration_cast<std::chrono::nanoseconds>
		(std::chrono::steady_clock::now() - mWorkThreadStartTime).count() * 0.000000001;
	return ToSamples(temp);
}

inline Samples Sound::Engine::ToSamples(const double Seconds)
{
	return (Samples)(Seconds * (double)SOUND_SAMPLE_RATE);
}

inline double Sound::Engine::ToSeconds(const Samples SampleCount)
{
	return (double)SampleCount / (double)SOUND_SAMPLE_RATE;
}

void Sound::Engine::_FillWithSinus(Samples CurrSample, Samples SampleCount, float* Buffer)
{
	const double FREQUENCY = 440.0;
	for (Samples i = 0; i < SampleCount; i++)
	{
		Buffer[i] = std::sinf(FREQUENCY * ToSeconds(CurrSample + i) * 2.0 * M_PI);
	}
}
