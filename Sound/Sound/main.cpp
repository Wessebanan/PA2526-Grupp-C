#include <iostream>
#include "AudioHelpers.h"
#include "AudioEngine.h"
int main()
{
	
	Audio::TimePoint start = std::chrono::steady_clock::now();
	Pa_Sleep(1000);
	Audio::TimePoint stop = std::chrono::steady_clock::now();
	auto time_passed = (stop - start).count();
	std::cout << time_passed << std::endl;
	return 0;
}