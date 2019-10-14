#include <iostream>
#include <fstream>
#include <chrono>
#include "AudioEngine.h"
typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;

inline
void SetupEngine(Audio::Engine& rEngine, Audio::Mixer& rMixer, Audio::PaHandler& rPaHandler)
{
	// Have the engine use this mixer
	rEngine.UseThisMixer(&rMixer);

	std::cout << "PortAudio Test: output sine wave. SR = " << SOUND_SAMPLE_RATE
		<< ", RingBufSize = " << SOUND_BUFFER_SIZE << std::endl;

	// Open stream
	rEngine.OpenStream();
	// Start feeding the sound card using the callback function
	rEngine.StartStream();

	rEngine.StartWorkThread();	// Start the work thread
								// to fill the ringbuffer
}

inline
void TeardownEngine(Audio::Engine& rEngine)
{
	rEngine.JoinWorkThread();	// End the work thread

	rEngine.StopStream();

	rEngine.CloseStream();
}

int main()
{
	//std::streampos size;
	//char* memblock;
	//TimePoint start = std::chrono::steady_clock::now();
	//TimePoint times[5];

	//std::ifstream file("C:/Users/Emil/Desktop/Dicey.mp3",
	//	std::ios::in | std::ios::binary | std::ios::ate);
	//times[0] = std::chrono::steady_clock::now();
	//if (file.is_open())
	//{
	//	size = file.tellg();
	//	times[1] = std::chrono::steady_clock::now();
	//	memblock = new char[size];
	//	times[2] = std::chrono::steady_clock::now();
	//	file.seekg(0, std::ios::beg);
	//	times[3] = std::chrono::steady_clock::now();
	//	file.read(memblock, size);
	//	times[4] = std::chrono::steady_clock::now();
	//	file.close();

	//	std::cout << "Entire file in memory" << std::endl;

	//	delete[] memblock;
	//}
	//else
	//	std::cout << "Unable to open file!\n";

	//for (int i = 0; i < 5; i++)
	//{
	//	std::cout << (times[i] - start).count() << std::endl;
	//}
	//std::cout << std::endl;
	//for (int i = 0; i < 4; i++)
	//{
	//	std::cout << (times[i+1] - times[i]).count() << std::endl;
	//}

	Audio::Engine engine;
	Audio::Mixer mixer;
	Audio::PaHandler pa_init;
	Audio::File::MusicData music_data;

	{
		using namespace Audio::Plugin;

		std::string path = "../Sound_test/cc_song.wav";
		music_data.Init(path);

		Audio::Music::Message message;
		message.flags = Audio::Music::M_REPLACE;
		message.pEntry = new Audio::Plugin::Streamer(&music_data);

		mixer.AddMusicMessage(message);
	}

	SetupEngine(engine, mixer, pa_init);
	Pa_Sleep(50000);
	TeardownEngine(engine);
	return 0;
}