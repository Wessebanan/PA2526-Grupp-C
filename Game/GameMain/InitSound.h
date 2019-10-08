#pragma once
#include "ecs.h"
#include "SoundGlobals.h"
#include "SoundECSSystems.h"
#include <iostream>

bool SetupEngine();
bool SetupBank();
void SetupEcs(ecs::EntityComponentSystem& rECS);

bool InitSound(ecs::EntityComponentSystem& rECS)
{
	if (soundPaHandler.result() != paNoError)
	{
		std::cerr << "An error occured while using the portaudio stream"
			<< "\nError number: " << soundPaHandler.result()
			<< "\nError message: " << Pa_GetErrorText(soundPaHandler.result())
			<< "\n";
		return false;
	}

	if(!SetupEngine())
	{
		std::cerr << "Setting up sound engine failed!\n";
		return false;
	}
	if (!SetupBank())
	{
		std::cerr << "Setting up sound bank failed!\n";
		return false;
	}
	return true;
}

bool SetupEngine()
{
	soundEngine.UseThisMixer(&soundMixer);

	if (!soundEngine.OpenStream())
	{
		std::cerr << "An error occured while opening the stream\n";
		return false;
	}

	if (!soundEngine.StartStream())
	{
		std::cerr << "An error occured while starting the stream\n";
		return false;
	}

	soundEngine.StartWorkThread();
	return true;
}

bool SetupBank()
{
	return soundBank.LoadMultipleFiles(SOUND_NAME_PATHS, 2);
}

void SetupEcs(ecs::EntityComponentSystem& rECS)
{
	rECS.createSystem<ecs::systems::SoundMessageSystem>(9);
}