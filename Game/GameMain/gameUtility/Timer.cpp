#include "Timer.h"

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::StartGame()
{
	mGameStart = high_resolution_clock::now();
}

void Timer::StartRound()
{
	mRoundStart = high_resolution_clock::now();
}

float Timer::GetGameTime()
{
	duration<float> round_time = high_resolution_clock::now() - mGameStart;
	return round_time.count();
}

float Timer::GetRoundTime()
{
	duration<float> round_time = high_resolution_clock::now() - mRoundStart;
	return round_time.count();
}

float Timer::GetFrameTime()
{	
	duration<float> frame_time = high_resolution_clock::now() - mFrameTime;
	mFrameTime = high_resolution_clock::now();
	return frame_time.count();
}
