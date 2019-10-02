#pragma once
#include <portaudio.h>
#include <thread>
#include "RingBuffer.h"

// Constant defines that doesn't have to change
#define SOUND_SAMPLE_RATE   (44100)
#define SOUND_BUFFER_SIZE  (512*2)	// Need to make this buffer
#define SOUND_FRAMES_PER_BUFFER (64)// size tighter
#define SOUND_MAX_CHANNELS (2)
#define SOUND_CHAIN_BUFFER_COUNT (2)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif

typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;
typedef unsigned long long Samples;
typedef Ringbuffer<std::pair<float,float>, SOUND_BUFFER_SIZE> FrameBuffer;

namespace Sound
{
	// Sound::Engine class 
	// The engine class is responsible for initializing
	// a sound stream and holds all functionallity needed
	// for transfering data from the chain buffers to the
	// sound card once the previous buffer is running low
	class Engine
	{
	public:
		Engine();

		// Initializes a stream by targeting the default
		// sound output
		bool OpenStream();
		bool OpenStream(PaDeviceIndex index);
		// Closest the stream
		bool CloseStream();
		// Starts the stream so that the callback function
		// begins being called
		bool StartStream();
		// Stops the stream so that the callback function
		// finishes being called
		bool StopStream();

		// ########### NEW NEW NEW ####################
		void StartWorkThread();
		void JoinWorkThread();

	protected:

	private:
		// This is the function that are called everytime
		// the sound card require a refill
		int PaCallbackMethod(const void* pInputBuffer, void* pOutputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* pTimeInfo,
			PaStreamCallbackFlags statusFlags);

		// This function redirects the callback function
		// to the member function above
		static int PaCallback(const void* pInputBuffer, void* pOutputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* pTimeInfo,
			PaStreamCallbackFlags statusFlags,
			void* pUserData);

		// Function called when the stream finishes
		// but currently have no use
		void PaStreamFinishedMethod();

		// This function redirects the callback function
		// to the member function above
		static void PaStreamFinished(void* pUserData);

		// PortAudio stream handler
		PaStream* mpStream;

		// The function to refill the ring buffer
		// with new data for the callback function
		// to consume
		void WorkerThreadUpdateMethod();
		// This function redirects the callback function
		// to the member function above
		// Temporarily doubles-up as a spin loop until
		// a proper thread-pool have been implemented
		static void WorkerThreadUpdate(void* data);

		// Helper functions to convert time to sample count
		inline Samples ToSamples(const float Seconds);
		inline float ToSeconds(const Samples SampleCount);
		// To get current sample count
		inline Samples GetWorkerCurrentSampleCount();


		std::thread* mpWorkerThread;
		TimePoint mWorkThreadStartTime;
		std::atomic_bool mWorkerThreadRun;

		FrameBuffer mBuffer;
		Samples mProducerLastSampleCount;

		// TEMPORARY
		// This function are to be removed once it's
		// not longer needed for testing purposes
		// Returns a float array of a sine wave
		// of varying length
		void _FillWithSinus(Samples CurrSample, Samples SampleCount, float* Buffer);
	};

	// Sound::PaHandler class
	// Just a handler instanced as an object to terminate
	// itself automatically when it goes out of scope
	class PaHandler
	{
	public:
		// Initialize on construction
		PaHandler()
			: _result(Pa_Initialize())
		{
		}
		// Terminate on destruction
		~PaHandler()
		{
			if (_result == paNoError)
			{
				Pa_Terminate();
			}
		}

		PaError result() const { return _result; }
	private:
		PaError _result;
	};
}