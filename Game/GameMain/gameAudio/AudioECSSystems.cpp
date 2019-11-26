#include "AudioECSSystems.h"
#include "AudioGlobals.h"
#include <iostream>

#include "AudioECSComponents.h"

ecs::systems::SoundMessageSystem::SoundMessageSystem()
{
	updateType = EventListenerOnly;
	mEngineInit = false;
	mSoundPaHandler = nullptr;
	mSoundEngine = nullptr;
	mSoundMixer = nullptr;
	mSoundBank = nullptr;
	for (int i = 0; i < AudioName::SOUND_COUNT; i++)
	{
		mSoundCooldownClock[i] = std::chrono::steady_clock::now();
	}
}

ecs::systems::SoundMessageSystem::~SoundMessageSystem()
{
	// Wait for the work thread to finish, the shutdown
	if (mEngineInit)
	{
		mSoundEngine->JoinWorkThread();
		mSoundEngine->StopStream();
		mSoundEngine->CloseStream();
	}

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

	mEngineInit = true;
	if (!SetupBank())
	{
		std::cerr << "Setting up sound bank failed!\n";
		return false;
	}

	//Subscribe to events
	{
		using namespace ecs::events;
		subscribeEventCreation(PlaySound::typeID);

		subscribeEventCreation(PlayMusic::typeID);
		subscribeEventCreation(PlaySecondaryMusic::typeID);
		subscribeEventCreation(PlaySubMusic::typeID);

		subscribeEventCreation(FadeInMusic::typeID);
		subscribeEventCreation(FadeInSecondaryMusic::typeID);
		subscribeEventCreation(FadeInSubMusic::typeID);

		subscribeEventCreation(FadeOutMusic::typeID);
		subscribeEventCreation(FadeOutSecondaryMusic::typeID);
		subscribeEventCreation(FadeOutSubMusic::typeID);

		subscribeEventCreation(MusicSetVolume::typeID);
		subscribeEventCreation(SecondaryMusicSetVolume::typeID);
		subscribeEventCreation(SubMusicSetVolume::typeID);
	}

	return true;
}

void ecs::systems::SoundMessageSystem::onEvent(TypeID _eventType, BaseEvent* _event)
{
	// Sort the messages
	{
		using namespace ecs::events;

		if (PlaySound::typeID == _eventType)
			ProcessPlaySound(static_cast<PlaySound*>(_event));

		else if (PlayMusic::typeID == _eventType)
			ProcessPlayMusic(static_cast<PlayMusic*>(_event));
		else if (PlaySecondaryMusic::typeID == _eventType)
			ProcessPlaySecondaryMusic(static_cast<PlaySecondaryMusic*>(_event));
		else if (PlaySubMusic::typeID == _eventType)
			ProcessPlaySubMusic(static_cast<PlaySubMusic*>(_event));

		else if (FadeInMusic::typeID == _eventType)
			ProcessFadeInMusic(static_cast<FadeInMusic*>(_event));
		else if (FadeInSecondaryMusic::typeID == _eventType)
			ProcessFadeInSecondaryMusic(static_cast<FadeInSecondaryMusic*>(_event));
		else if (FadeInSubMusic::typeID == _eventType)
			ProcessFadeInSubMusic(static_cast<FadeInSubMusic*>(_event));

		else if (FadeOutMusic::typeID == _eventType)
			ProcessFadeOutMusic(static_cast<FadeOutMusic*>(_event));
		else if (FadeOutSecondaryMusic::typeID == _eventType)
			ProcessFadeOutSecondaryMusic(static_cast<FadeOutSecondaryMusic*>(_event));
		else if (FadeOutSubMusic::typeID == _eventType)
			ProcessFadeOutSubMusic(static_cast<FadeOutSubMusic*>(_event));

		else if (MusicSetVolume::typeID == _eventType)
			ProcessMusicSetVolume(static_cast<MusicSetVolume*>(_event));
		else if (SecondaryMusicSetVolume::typeID == _eventType)
			ProcessSecondaryMusicSetVolume(static_cast<SecondaryMusicSetVolume*>(_event));
		else if (SubMusicSetVolume::typeID == _eventType)
			ProcessSubMusicSetVolume(static_cast<SubMusicSetVolume*>(_event));
	}
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
		mSoundEngine->CloseStream();
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
	const long long COOLDOWN = 400000000;
	// Check if entity is already making a sound
	// Always allow SCREAM_SOUND to play, even it the entity is already playing a sound.
	//if (pEvent->audioName != SOUND_scream && pEvent->invokerEntityId != 0)
	//{
	//	Entity* p_entity = getEntity(pEvent->invokerEntityId);

	//	// Don't play sound if entity don't exist, or the entity already has
	//	// a SoundCooldownComponent (== is already playing a sound within the last
	//	// SOUND_COOLDOWN seconds).
	//	if (!p_entity || (p_entity->hasComponentOfType<components::SoundCooldownComponent>()))
	//	{
	//		return;
	//	}

	//	components::SoundCooldownComponent cooldown;
	//	cooldown.timeElapsed = 0;
	//	createComponent(p_entity->getID(), cooldown);
	//}
	//std::cout << (std::chrono::steady_clock::now() - mSoundCooldownClock[pEvent->audioName]).count()
	//	<< std::endl << COOLDOWN << std::endl;
	if ((std::chrono::steady_clock::now() - mSoundCooldownClock[pEvent->audioName]).count() >= COOLDOWN)
	{
		std::cout << "Yes\n";
		Audio::FileData* temp_data = (*mSoundBank)[pEvent->audioName];
		if (temp_data == nullptr)
		{
			// No point in playing nothing
			return;
		}
		bool temp_loop_bool = pEvent->soundFlags & SoundFlags::SF_REPEAT;
		float pitch = 1.0f;
		if (pEvent->soundFlags & SoundFlags::SF_RANDOM_PITCH)
		{
			pitch = (rand() % 1000) / 4000.f + 0.85f;
		}
		Audio::Plugin::Plugin* temp_plugin = new Audio::Plugin::Sampler(temp_data, (temp_loop_bool ? 0 : 1), pitch);
		mSoundMixer->AddSoundMessage({ temp_plugin });

		mSoundCooldownClock[pEvent->audioName] = std::chrono::steady_clock::now();
	}
}

void ecs::systems::SoundMessageSystem::ProcessPlayMusic(ecs::events::PlayMusic* pEvent)
{
	mSoundMixer->AddMusicMessage({
		(*mSoundBank)[pEvent->audioName],
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_REPLACE_MUSIC |
		Audio::Music::M_SYNC_OTHER_WITH_THIS
	});
}

void ecs::systems::SoundMessageSystem::ProcessPlaySecondaryMusic(ecs::events::PlaySecondaryMusic* pEvent)
{
	mSoundMixer->AddMusicMessage({
		(*mSoundBank)[pEvent->audioName],
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_REPLACE_MUSIC |
		Audio::Music::M_TARGET_SECONDARY |
		Audio::Music::M_SYNC_OTHER_WITH_THIS
	});
}

void ecs::systems::SoundMessageSystem::ProcessPlaySubMusic(ecs::events::PlaySubMusic* pEvent)
{
	mSoundMixer->AddMusicMessage({
		(*mSoundBank)[pEvent->audioName],
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_REPLACE_MUSIC |
		Audio::Music::M_TARGET_SUB |
		Audio::Music::M_SYNC_OTHER_WITH_THIS
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

void ecs::systems::SoundMessageSystem::ProcessFadeInSecondaryMusic(ecs::events::FadeInSecondaryMusic* pEvent)
{
	mSoundMixer->AddMusicMessage({
		(unsigned long)(pEvent->fadeInTimeInSeconds * 44100.f),
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_FADE_IN |
		Audio::Music::M_TARGET_SECONDARY
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

void ecs::systems::SoundMessageSystem::ProcessFadeOutMusic(ecs::events::FadeOutMusic* pEvent)
{
	mSoundMixer->AddMusicMessage({
		(unsigned long)(pEvent->fadeOutTimeInSeconds * 44100.f),
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_FADE_OUT
	});
}

void ecs::systems::SoundMessageSystem::ProcessFadeOutSecondaryMusic(ecs::events::FadeOutSecondaryMusic* pEvent)
{
	mSoundMixer->AddMusicMessage({
		(unsigned long)(pEvent->fadeOutTimeInSeconds * 44100.f),
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_FADE_OUT |
		Audio::Music::M_TARGET_SECONDARY
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

void ecs::systems::SoundMessageSystem::ProcessMusicSetVolume(ecs::events::MusicSetVolume* pEvent)
{
	mSoundMixer->AddMusicMessage({
		pEvent->volume,
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_SET_GAIN
		});
}

void ecs::systems::SoundMessageSystem::ProcessSecondaryMusicSetVolume(ecs::events::SecondaryMusicSetVolume* pEvent)
{
	mSoundMixer->AddMusicMessage({
		pEvent->volume,
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_SET_GAIN |
		Audio::Music::M_TARGET_SECONDARY
	});
}

void ecs::systems::SoundMessageSystem::ProcessSubMusicSetVolume(ecs::events::SubMusicSetVolume* pEvent)
{
	mSoundMixer->AddMusicMessage({
		pEvent->volume,
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_SET_GAIN |
		Audio::Music::M_TARGET_SUB
		});
}

//ecs::systems::SoundCooldownClearSystem::SoundCooldownClearSystem()
//{
//	updateType = EntityUpdate;
//	typeFilter.addRequirement(components::SoundCooldownComponent::typeID);
//}
//
//ecs::systems::SoundCooldownClearSystem::~SoundCooldownClearSystem()
//{
//	//
//}
//
//void ecs::systems::SoundCooldownClearSystem::updateEntity(FilteredEntity& rEntity, float delta)
//{
//	components::SoundCooldownComponent* p_cooldown = rEntity.getComponent<components::SoundCooldownComponent>();
//
//	p_cooldown->timeElapsed += delta;
//
//	if (p_cooldown->timeElapsed >= SOUND_COOLDOWN)
//	{
//		removeComponent(p_cooldown->getEntityID(), components::SoundCooldownComponent::typeID);
//	}
//}
