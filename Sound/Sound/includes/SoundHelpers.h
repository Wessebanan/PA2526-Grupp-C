#pragma once
#include <chrono>
#include "RingBuffer.h"

// Constant defines that doesn't have to change
#define SOUND_SAMPLE_RATE   (44100)
#define SOUND_BUFFER_SIZE  (512*2)	// TODO: make this buffer
#define SOUND_FRAMES_PER_BUFFER (64)// size tighter (=less latency)
#define SOUND_MAX_CHANNELS (2)
#define SOUND_CHAIN_BUFFER_COUNT (2)
#define SOUND_LEFT (0)
#define SOUND_RIGHT (1)
#define SOUND_MAX_VOICES (12)
#define SOUND_MAX_BANK_FILES (10)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif

namespace Sound
{
	// Defining a frame structure
	struct Frame
	{
		float left;
		float right;
	};
	// typedef commonly used types to simplify
	typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;
	typedef Ringbuffer<Frame, SOUND_BUFFER_SIZE> FrameBuffer;
	typedef unsigned long long Samples;

	// Helper functions to convert time to sample count
	Samples ToSamples(const float Seconds);
	float ToSeconds(const Samples SampleCount);
}