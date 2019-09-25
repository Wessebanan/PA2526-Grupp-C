#pragma once
#include <portaudio.h>

// Constant defines that doesn't have to change
#define SOUND_SAMPLE_RATE   (44100)
#define SOUND_FRAMES_PER_BUFFER  (64)
#define SOUND_MAX_CHANNELS (2)
#define SOUND_CHAIN_BUFFER_COUNT (2)

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
		int Channels;
		int Frames;
		float* Data[SOUND_MAX_CHANNELS];

		Buffer(int Channels, int Frames)
		{
			this->Channels = Channels;
			this->Frames = Frames;

			// Initialize a float array for every channel
			for (int i = 0; i < Channels; i++)
			{
				this->Data[i] = new float[(long long)Channels * Frames];
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

	protected:
		// These functions are only for testing purposes
		// and are only meant to be used by
		// SoundEngineTester. If someone can come up with
		// a better way of doing this, please do tell me :)
		Buffer* __GetChainBuffer(int Index);

	private:
		// This is the function that are called everytime
		// the sound card require a refill
		int PaCallbackMethod(const void* inputBuffer, void* outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags);

		// This function redirects the callback function
		// to the member function above
		static int PaCallback(const void* inputBuffer, void* outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void* userData);

		// Function called when the stream finishes
		// but currently have no use
		void PaStreamFinishedMethod();

		// This function redirects the callback function
		// to the member function above
		static void PaStreamFinished(void* userData);

		// PortAudio stream handler
		PaStream* mpStream;
		// Chain buffers to be consumed and filled in
		// a loop
		Buffer* mpChainBuffers[SOUND_CHAIN_BUFFER_COUNT];
		// Index of the chain buffer to be consumed
		unsigned int mConsumeBufferIndex;
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