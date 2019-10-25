#pragma once
#include <chrono>

using namespace std::chrono;
class Timer
{
private:
	high_resolution_clock::time_point mGameStart;
	high_resolution_clock::time_point mRoundStart;
	high_resolution_clock::time_point mFrameTime;

public:
	Timer();
	~Timer();

	// Starts game timer.
	void StartGame();
	// Starts round timer.
	void StartRound();
	// Returns total game time as seconds.
	float GetGameTime();
	// Returns total round time as seconds.
	float GetRoundTime();
	// Returns time since last call to this function as seconds.
	float GetFrameTime();
};
