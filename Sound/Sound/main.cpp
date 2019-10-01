#include "SoundEngine.h"

int main()
{
	Sound::Engine engine;
	Sound::PaHandler pa_init;
	
	engine.OpenStream();
	engine.StartStream();
	engine.StartWorkThread();
	Pa_Sleep(5000);
	engine.JoinWorkThread();
	engine.StopStream();
	engine.CloseStream();

	return 0;
}