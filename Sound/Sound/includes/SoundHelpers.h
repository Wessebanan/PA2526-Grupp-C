#pragma once
#include <chrono>
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

namespace Sound
{

	typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;
	typedef Ringbuffer<std::pair<float, float>, SOUND_BUFFER_SIZE> FrameBuffer;
	typedef unsigned long long Samples;

	// Helper functions to convert time to sample count
	Samples ToSamples(const float Seconds);
	float ToSeconds(const Samples SampleCount);
}