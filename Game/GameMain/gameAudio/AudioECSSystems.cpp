#include "AudioECSSystems.h"
#include "AudioGlobals.h"
#include <iostream>

#include "AudioECSComponents.h"

#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"

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

	if ((std::chrono::steady_clock::now() - mSoundCooldownClock[pEvent->audioName]).count() >= COOLDOWN)
	{
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

ecs::systems::BattleMusicIntensitySystem::BattleMusicIntensitySystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(ecs::components::MoveStateComponent::typeID);
	typeFilter.addRequirement(ecs::components::TransformComponent::typeID);
}

ecs::systems::BattleMusicIntensitySystem::~BattleMusicIntensitySystem()
{
}

void ecs::systems::BattleMusicIntensitySystem::Init()
{
	components::BattleMusicIntensityComponent* p_comp = 
		static_cast<components::BattleMusicIntensityComponent*>(
			getComponentsOfType<components::BattleMusicIntensityComponent>().next());

	mSoundMetaEntity = p_comp->getEntityID();
}


void ecs::systems::BattleMusicIntensitySystem::updateEntity(FilteredEntity& entity, float delta)
{
	ecs::components::TransformComponent* p_transform =
		entity.getComponent<ecs::components::TransformComponent>();
	ecs::components::MoveStateComponent* p_move_state =
		entity.getComponent<ecs::components::MoveStateComponent>();

	components::BattleMusicIntensityComponent* p_comp = getComponentFromKnownEntity<components::BattleMusicIntensityComponent>(mSoundMetaEntity);

	// Check if the entity is currently ordered to attack another unit
	if (p_move_state->activeCommand == STATE::ATTACK)
	{
		Entity* enemy_entity;
		// Check if the enemy target is a valid entity
		if (enemy_entity = getEntity(p_move_state->goalID))
		{
			components::TransformComponent* p_enemy_transform =
				getComponentFromKnownEntity<components::TransformComponent>(enemy_entity->getID());
			// Add the distance towards the target
			p_comp->totalDistance += hypotf(
				p_transform->position.x - p_enemy_transform->position.x,
				p_transform->position.z - p_enemy_transform->position.z);
			// Count how many is in attack state
			p_comp->totalCount++;
		}
	}
}

ecs::systems::SubTrackUpdateSystem::SubTrackUpdateSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(components::BattleMusicIntensityComponent::typeID);
}

ecs::systems::SubTrackUpdateSystem::~SubTrackUpdateSystem()
{
}

void ecs::systems::SubTrackUpdateSystem::updateEntity(FilteredEntity& entity, float delta)
{
	// How fast the drumtrack should transition between 
	// intense or calm
	const float SMOOTH_FACTOR = 0.99f;

	components::BattleMusicIntensityComponent* p_comp =
		entity.getComponent<ecs::components::BattleMusicIntensityComponent>();

	events::SubMusicSetVolume m_event;

	// If at least someone is in attack mode
	if (p_comp->totalCount != 0)
	{
		// The previous intensity gets blended by how close
		// attacking units are to each other this frame,
		// scaled within a certain radius
		p_comp->currentIntensity = p_comp->currentIntensity * SMOOTH_FACTOR + (1.0f - SMOOTH_FACTOR) *
			(1.0f - (fminf(fmaxf(p_comp->totalDistance / (float)p_comp->totalCount, 2.0f), 8.0f) - 2.0f) / 6.0f);
		// Send current intensity as volume
		m_event.volume = p_comp->currentIntensity;
		createEvent(m_event);
	}
	else
	{
		// If no one is attacking, start silencing
		// the drumtrack
		p_comp->currentIntensity *= SMOOTH_FACTOR;
		m_event.volume = p_comp->currentIntensity;
		createEvent(m_event);
	}
	// Set the distance calculation to 0 for next frame
	p_comp->totalDistance = 0.0f;
	p_comp->totalCount = 0;
}
