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
	high_resolution_clock::time_point now = high_resolution_clock::now();
	duration<float> frame_time = now - mFrameTime;
	mFrameTime = now;
	return frame_time.count();
}
