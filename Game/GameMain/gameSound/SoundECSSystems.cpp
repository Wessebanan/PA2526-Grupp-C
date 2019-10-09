#include "SoundECSSystems.h"
#include "SoundGlobals.h"
#include <iostream>

ecs::systems::SoundMessageSystem::SoundMessageSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::PlaySound::typeID);
	typeFilter.addRequirement(ecs::events::PlayMusic::typeID);
}

ecs::systems::SoundMessageSystem::~SoundMessageSystem()
{
	// Wait for the work thread to finish, the shutdown
	mSoundEngine->JoinWorkThread();
	mSoundEngine->StopStream();
	mSoundEngine->CloseStream();

	if (mSoundPaHandler != nullptr)
	{
		delete mSoundPaHandler;
		mSoundPaHandler = nullptr;
	}
	if (mSoundEngine != nullptr)
	{
		delete mSoundEngine;
			mSoundEngine = nullptr;
	}
	if (mSoundMixer != nullptr)
	{
		delete mSoundMixer;
		mSoundMixer = nullptr;
	}
	if (mSoundBank != nullptr)
	{
		delete mSoundBank;
		mSoundBank = nullptr;
	}
}

bool ecs::systems::SoundMessageSystem::Init()
{
	// Initialize portaudio
	mSoundPaHandler = new Sound::PaHandler();
	if (mSoundPaHandler->result() != paNoError)
	{
		std::cerr << "An error occured while using the portaudio stream"
			<< "\nError number: " << mSoundPaHandler->result()
			<< "\nError message: " << Pa_GetErrorText(mSoundPaHandler->result())
			<< "\n";
		return false;
	}

	// Create a new mixer
	mSoundMixer = new Sound::Mixer();

	if (!SetupEngine())
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

void ecs::systems::SoundMessageSystem::readEvent(BaseEvent& rEvent, float delta)
{
	// TODO:
	// This is how it will be implemented in a future task
	//if (rEvent.getTypeID = ecs::events::PlaySound::typeID)
	//{
	//	ecs::events::PlaySound* p_event = static_cast<ecs::events::PlaySound*>(&rEvent);
	//	// ...
	//}
	//else
	//{
	//	ecs::events::PlayMusic* p_event = static_cast<ecs::events::PlayMusic*>(&rEvent);
	//	// ...
	//}
	
	// TEMPORARY: PlaySound and PlayMusic is the same for now
	ecs::events::PlaySound* p_event = static_cast<ecs::events::PlaySound*>(&rEvent);
	Sound::FileData* temp_data = (*mSoundBank)[p_event->soundName];
	if (temp_data == nullptr)
	{
		// No point in playing nothing
		return;
	}
	bool temp_loop_bool = p_event->soundFlags & SoundFlags::REPEAT;
	Sound::Plugin::Plugin* temp_plugin = new Sound::Plugin::Sampler(temp_data, (temp_loop_bool ? 0 : 1));
	mSoundMixer->AddSoundMessage({ temp_plugin });
}

bool ecs::systems::SoundMessageSystem::SetupEngine()
{
	// Create a new engine
	mSoundEngine = new Sound::Engine();
	// Use the mixer as the engines mixer
	mSoundEngine->UseThisMixer(mSoundMixer);

	// Open the portaudio stream
	if (!mSoundEngine->OpenStream())
	{
		std::cerr << "An error occured while opening the stream\n";
		return false;
	}

	// Start the stream
	// Will start triggering the callback function
	if (!mSoundEngine->StartStream())
	{
		std::cerr << "An error occured while starting the stream\n";
		return false;
	}

	// Initialize and start the work thread
	mSoundEngine->StartWorkThread();
	return true;
}

bool ecs::systems::SoundMessageSystem::SetupBank()
{
	// Create a new bank and load all sound effects
	mSoundBank = new Sound::Bank();
	return mSoundBank->LoadMultipleFiles(SOUND_NAME_PATHS, sizeof(SOUND_NAME_PATHS) / sizeof(std::string));
}