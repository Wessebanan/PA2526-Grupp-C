#include <iostream>
#include <fstream>
#include <chrono>
typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;

int main()
{
	std::streampos size;
	char* memblock;
	TimePoint start = std::chrono::steady_clock::now();
	TimePoint times[5];

	std::ifstream file("C:/Users/Emil/Desktop/Dicey.mp3",
		std::ios::in | std::ios::binary | std::ios::ate);
	times[0] = std::chrono::steady_clock::now();
	if (file.is_open())
	{
		size = file.tellg();
		times[1] = std::chrono::steady_clock::now();
		memblock = new char[size];
		times[2] = std::chrono::steady_clock::now();
		file.seekg(0, std::ios::beg);
		times[3] = std::chrono::steady_clock::now();
		file.read(memblock, size);
		times[4] = std::chrono::steady_clock::now();
		file.close();

		std::cout << "Entire file in memory" << std::endl;

		delete[] memblock;
	}
	else
		std::cout << "Unable to open file!\n";

	for (int i = 0; i < 5; i++)
	{
		std::cout << (times[i] - start).count() << std::endl;
	}
	std::cout << std::endl;
	for (int i = 0; i < 4; i++)
	{
		std::cout << (times[i+1] - times[i]).count() << std::endl;
	}

	return 0;
}