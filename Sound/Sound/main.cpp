#include <iostream>
#include "AudioHelpers.h"
#include "AudioEngine.h"
int main()
{
	
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	Pa_Sleep(1000);
	std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
	auto time_passed = (stop - start).count();
	std::cout << time_passed << std::endl;
	return 0;
}