#pragma once
#include <portaudio.h>
#include <thread>
#include "RingBuffer.h"

// Constant defines that doesn't have to change
#define SOUND_SAMPLE_RATE   (44100)
#define SOUND_BUFFER_SIZE  (512*4)
#define SOUND_FRAMES_PER_BUFFER (64)
#define SOUND_MAX_CHANNELS (2)
#define SOUND_CHAIN_BUFFER_COUNT (2)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif

typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;
typedef unsigned long long Samples;
//typedef Ringbuffer<std::pair<float,float>, SOUND_BUFFER_SIZE> FrameBuffer;
typedef Ringbuffer<std::pair<float,float>, SOUND_BUFFER_SIZE> FrameBuffer;

namespace Sound
{
	// Sound::Buffer struct
	// All sample data are stored in float arrays,
	// one array per channel (e.g. 2 channels for stereo).
	// This struct dynamically allocates the approproite
	// amount of float arrays depending on channels and
	// frames.
	struct Buffer
	{
		int channels;
		int frames;
		float* Data[SOUND_MAX_CHANNELS];
		Buffer(int channels, int frames)
		{
			this->channels = channels;
			this->frames = frames;

			// Initialize a float array for every channel
			for (int i = 0; i < channels; i++)
			{
				this->Data[i] = new float[(long long)channels * frames];
			}
		}
	};


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
		// These functions are only for testing purposes
		// and are only meant to be used by
		// SoundEngineTester. If someone can come up with
		// a better way of doing this, please do tell me :)
		Buffer* __GetChainBuffer(int index);

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

		// ########### NEW NEW NEW ####################
		FrameBuffer mBuffer;

		void WorkerThreadUpdateMethod();
		static void WorkerThreadUpdate(void* data);
		std::atomic_bool mWorkerThreadRun;

		std::thread* mpWorkerThread;
		TimePoint mWorkThreadStartTime;
		Samples mLastSampleCount;
		inline Samples GetWorkerCurrentSampleCount();
		inline Samples ToSamples(const double Seconds);
		inline double ToSeconds(const Samples SampleCount);

		void _FillWithSinus(Samples CurrSample, Samples SampleCount, float* Buffer);

		// Chain buffers to be consumed and filled in
		// a loop
		//Buffer* mpChainBuffers[SOUND_CHAIN_BUFFER_COUNT];
		// Index of the chain buffer to be consumed
		//unsigned int mConsumeBufferIndex;
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