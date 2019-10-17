#include "AudioECSSystems.h"
#include "AudioGlobals.h"
#include <iostream>

ecs::systems::SoundMessageSystem::SoundMessageSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::PlaySound::typeID);
	typeFilter.addRequirement(ecs::events::PlayMusic::typeID);
	typeFilter.addRequirement(ecs::events::FadeInMusic::typeID);
	typeFilter.addRequirement(ecs::events::PlaySubMusic::typeID);
	typeFilter.addRequirement(ecs::events::FadeInSubMusic::typeID);
	typeFilter.addRequirement(ecs::events::FadeOutSubMusic::typeID);
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
	mSoundPaHandler = new Audio::PaHandler();
	if (mSoundPaHandler->result() != paNoError)
	{
		std::cerr << "An error occured while using the portaudio stream"
			<< "\nError number: " << mSoundPaHandler->result()
			<< "\nError message: " << Pa_GetErrorText(mSoundPaHandler->result())
			<< "\n";
		return false;
	}

	// Create a new mixer
	mSoundMixer = new Audio::Mixer();

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
	// Sort the messages
	{
		using namespace ecs::events;
		//switch (rEvent.getTypeID())
		//{
		//case PlaySound::typeID:
		//	ProcessPlaySound(static_cast<PlaySound*>(&rEvent));
		//	break;
		//case PlayMusic::typeID:
		//	ProcessPlayMusic(static_cast<PlayMusic*>(&rEvent));
		//	break;
		//case FadeInMusic::typeID:
		//	FadeInMusic(static_cast<FadeInMusic*>(&rEvent));
		//	break;
		//}
		TypeID id = rEvent.getTypeID();

		if (PlaySound::typeID == id)
			ProcessPlaySound(static_cast<PlaySound*>(&rEvent));
		else if (PlayMusic::typeID == id)
			ProcessPlayMusic(static_cast<PlayMusic*>(&rEvent));
		else if (FadeInMusic::typeID == id)
			ProcessFadeInMusic(static_cast<FadeInMusic*>(&rEvent));
		else if (PlaySubMusic::typeID == id)
			ProcessPlaySubMusic(static_cast<PlaySubMusic*>(&rEvent));
		else if (FadeInSubMusic::typeID == id)
			ProcessFadeInSubMusic(static_cast<FadeInSubMusic*>(&rEvent));
		else if (FadeOutSubMusic::typeID == id)
			ProcessFadeOutSubMusic(static_cast<FadeOutSubMusic*>(&rEvent));
		else
			std::cout << "HEY!";
	}
	//if (rEvent.getTypeID() == ecs::events::)
	//{
	//	ecs::events::PlaySound* p_event = static_cast<ecs::events::PlaySound*>(&rEvent);
	//	ProcessPlaySound(p_event);
	//}
	//else
	//{
	//	ecs::events::PlayMusic* p_event = static_cast<ecs::events::PlayMusic*>(&rEvent);
	//	ProcessPlayMusic(p_event);
	//}
}

bool ecs::systems::SoundMessageSystem::SetupEngine()
{
	// Create a new engine
	mSoundEngine = new Audio::Engine();
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
	mSoundBank = new Audio::Bank();
	return mSoundBank->LoadMultipleFiles(AUDIO_NAME_PATHS, sizeof(AUDIO_NAME_PATHS) / sizeof(std::string));
}

void ecs::systems::SoundMessageSystem::ProcessPlaySound(ecs::events::PlaySound* pEvent)
{
	Audio::FileData* temp_data = (*mSoundBank)[pEvent->audioName];
	if (temp_data == nullptr)
	{
		// No point in playing nothing
		return;
	}
	bool temp_loop_bool = pEvent->soundFlags & SoundFlags::SF_REPEAT;
	Audio::Plugin::Plugin* temp_plugin = new Audio::Plugin::Sampler(temp_data, (temp_loop_bool ? 0 : 1));
	mSoundMixer->AddSoundMessage({ temp_plugin });
}

void ecs::systems::SoundMessageSystem::ProcessPlayMusic(ecs::events::PlayMusic* pEvent)
{
	//bool temp_replace_bool = pEvent->musicFlags & MusicFlags::MF_REPLACE;
	//Audio::Plugin::Plugin* temp_plugin = new Audio::Plugin::Sampler(temp_data, 0);
	mSoundMixer->AddMusicMessage({
		(*mSoundBank)[pEvent->audioName],
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_REPLACE_MUSIC
	});
}

void ecs::systems::SoundMessageSystem::ProcessFadeInMusic(ecs::events::FadeInMusic* pEvent)
{
	mSoundMixer->AddMusicMessage({
		(unsigned long)(pEvent->fadeInTimeInSeconds*44100.f),
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_FADE_IN
	});
}

void ecs::systems::SoundMessageSystem::ProcessPlaySubMusic(ecs::events::PlaySubMusic* pEvent)
{
	mSoundMixer->AddMusicMessage({
		(*mSoundBank)[pEvent->audioName],
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_REPLACE_MUSIC |
		Audio::Music::M_TARGET_SUB |
		Audio::Music::M_SYNC_THIS_WITH_OTHER
	});
}

void ecs::systems::SoundMessageSystem::ProcessFadeInSubMusic(ecs::events::FadeInSubMusic* pEvent)
{
	mSoundMixer->AddMusicMessage({
		(unsigned long)(pEvent->fadeInTimeInSeconds * 44100.f),
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_FADE_IN |
		Audio::Music::M_TARGET_SUB
		});
}

void ecs::systems::SoundMessageSystem::ProcessFadeOutSubMusic(ecs::events::FadeOutSubMusic* pEvent)
{
	mSoundMixer->AddMusicMessage({
		(unsigned long)(pEvent->fadeOutTimeInSeconds * 44100.f),
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_FADE_OUT |
		Audio::Music::M_TARGET_SUB
	});
}
