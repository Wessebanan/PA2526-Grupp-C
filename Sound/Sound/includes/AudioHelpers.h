#pragma once
#include <chrono>
#include "RingBuffer.h"

// Constant defines that doesn't have to change
#define SOUND_SAMPLE_RATE   (44100)
#define SOUND_BUFFER_SIZE  (512*4)	// TODO: make this buffer
#define SOUND_FRAMES_PER_BUFFER (64)// size tighter (=less latency)
#define SOUND_MAX_CHANNELS (2)
#define SOUND_CHAIN_BUFFER_COUNT (2)
#define SOUND_LEFT (0)
#define SOUND_RIGHT (1)
#define SOUND_MAX_SOUND_VOICES (12)
#define SOUND_MAX_MUSIC_VOICES (2)
#define SOUND_MAX_BANK_FILES (10)
#define SOUND_MAX_SOUND_MESSAGES (16)
#define SOUND_MAX_MUSIC_MESSAGES (4)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif

namespace Audio
{
	// Defining a frame structure
	struct Frame
	{
		float left;
		float right;
	};
	// typedef commonly used types to simplify
	typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;
	typedef Ringbuffer<Frame, SOUND_BUFFER_SIZE, true> FrameBuffer;
	typedef unsigned long long Samples;

	// Helper functions to convert time to sample count
	Samples ToSamples(const float seconds);
	float ToSeconds(const Samples sampleCount);
}