#pragma once
#include <portaudio.h>
#include <thread>
#include "SoundHelpers.h"
#include "Plugin.h"
#include "Mixer.h"

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
		~Engine();

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

		void StartWorkThread();
		void JoinWorkThread();

		// Sets a mixer to get frame data from
		void UseThisMixer(Mixer* pMixer);

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
		static void WorkerThreadUpdate(void* pData);

		// To get current sample count
		inline Samples GetWorkerCurrentSampleCount();


		std::thread* mpWorkerThread;
		TimePoint mWorkThreadStartTime;
		std::atomic_bool mWorkerThreadRun;

		FrameBuffer mBuffer;
		Samples mProducerLastSampleCount;

		Mixer* mpMixer;
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