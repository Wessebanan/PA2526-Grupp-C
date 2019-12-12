#include "AudioECSSystems.h"
#include "AudioGlobals.h"
#include <iostream>

#include "AudioECSComponents.h"

#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameGameLoop/GameLoopEvents.h"
#include "../gameGameLoop/GameLoopComponents.h"
#include "../gameAnimation/AnimationComponents.h"
#include "../gameWorld/WorldComponents.h"
#undef PlaySound

ecs::systems::SoundMessageSystem::SoundMessageSystem()
{
	updateType = Actor;
	mEngineInit = false;
	mSoundPaHandler = nullptr;
	mSoundEngine = nullptr;
	mSoundMixer = nullptr;
	mSoundBank = nullptr;
	for (int i = 0; i < AudioName::SOUND_COUNT; i++)
	{
		mSoundCooldownClock[i] = std::chrono::steady_clock::now();
	}
	mCurrentBeat = 0;
	mBeatTime = 1.0f;
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

		subscribeEventCreation(SetMusicSpeed::typeID);
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

		else if (SetMusicSpeed::typeID == _eventType)
			ProcessSetMusicSpeed(static_cast<SetMusicSpeed*>(_event));
	}
}

void ecs::systems::SoundMessageSystem::act(float _delta)
{
	// How many samples a beat represents
	static const float SAMPLES_TO_BEAT = 44100.f * (60.f / 65.f);

	// Check how many beats we're currently at
	int temp = (int)((float)mSoundMixer->GetCurrentSampleFromMainMusic() / SAMPLES_TO_BEAT);
	
	// If there's a difference since last time...
	if (temp != mCurrentBeat)
	{
		// Reset the bobble timer to play the animation
		mCurrentBeat = temp;
		mBeatTime = 0.0f;
	}

	// While the time is not up, play the animation
	if (mBeatTime < 1.0f)
	{
		// The head extention are calculated using a bezier curve approach
		float reverse = (1.0f - mBeatTime);
		float extend = ((3.0f * mBeatTime * 2.0f) + (3.0f * reverse * -4.0f) + (reverse * reverse * -1.0f)) * mBeatTime * reverse;
		// Pre-compute a translation matrix
		DirectX::XMMATRIX transform_matrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0.f, 0.f, extend));
		// Fetch all skeleton data
		ecs::ComponentIterator c_it = ecs::ECSUser::getComponentsOfType<components::SkeletonComponent>();
		components::SkeletonComponent* temp_comp;
		// For every skeleton, take the head and apply our matrix on it
		while (temp_comp = static_cast<components::SkeletonComponent*>(c_it.next()))
		{
			DirectX::XMMATRIX temp_matrix = DirectX::XMLoadFloat4x4(&temp_comp->skeletonData.frameData[2]);
			temp_matrix = DirectX::XMMatrixMultiply(temp_matrix, transform_matrix);
			DirectX::XMStoreFloat4x4(&temp_comp->skeletonData.frameData[2], temp_matrix);
		}
		// Progress animation
		mBeatTime += _delta * 3.f;
	}

	float freq_array[256];
	mSoundMixer->GetMusicManager()->GetFrequencies(freq_array);
	components::WaveCenterComponent* wave_comp = 
		static_cast<components::WaveCenterComponent*>(ecs::ECSUser::getComponentsOfType<components::WaveCenterComponent>().next());
	static const unsigned char freq = 0;
	// This is for testing and debugging frequency ranges
	//if (GetAsyncKeyState('K'))
	//{
	//	freq++;
	//	std::cout << (int)freq << std::endl;
	//}
	//else if (GetAsyncKeyState('J'))
	//{
	//	freq--;
	//	std::cout << (int)freq << std::endl;
	//}
	if(freq_array[freq] < 1.0f && freq_array[freq] > 0.0f)
		*wave_comp->mpFirstElement = freq_array[freq] * 40.f;
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

void ecs::systems::SoundMessageSystem::ProcessSetMusicSpeed(ecs::events::SetMusicSpeed* pEvent)
{
	mSoundMixer->AddMusicMessage({
		pEvent->speed,
		Audio::Music::M_DATA_AS_PARAMETER |
		Audio::Music::M_FUNC_SET_SPEED
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

ecs::systems::MusicSpeedSystem::MusicSpeedSystem()
{
	updateType = Actor;
	subscribeEventCreation(events::MusicSpeedGoal::typeID);

	mGoal = mCurrent = 1.0f;
}

void ecs::systems::MusicSpeedSystem::onEvent(TypeID _eventType, BaseEvent* _event)
{
	events::MusicSpeedGoal* event = static_cast<events::MusicSpeedGoal*>(_event);
	mGoal = event->speed;
}

void ecs::systems::MusicSpeedSystem::act(float _delta)
{
	if (mCurrent != mGoal)
	{
		const float ratio = 0.1f;
		if (signbit(mCurrent - mGoal))
		{
			mCurrent += _delta * ratio;
			if (mCurrent > mGoal)
				mCurrent = mGoal;
		}
		else
		{
			mCurrent -= _delta * ratio;
			if (mCurrent < mGoal)
				mCurrent = mGoal;
		}
		events::SetMusicSpeed m_event;
		m_event.speed = mCurrent;
		createEvent(m_event);
	}
}

ecs::systems::SpeedUpOnRoundEnd::SpeedUpOnRoundEnd()
{
	updateType = SystemUpdateType::EventListenerOnly;
	subscribeEventCreation(events::RoundEndEvent::typeID);
}

void ecs::systems::SpeedUpOnRoundEnd::onEvent(TypeID _eventType, BaseEvent* _event)
{
	const float speed_up_factor = 0.05f;
	components::GameLoopComponent* p_comp =
		static_cast<components::GameLoopComponent*>(
			getComponentsOfType<components::GameLoopComponent>().next());

	int total = 0;
	for (int i = 0; i < 4; i++)
	{
		total += p_comp->mPlayerPoints[i];
	}
	events::MusicSpeedGoal m_event;
	m_event.speed = 1.0f + speed_up_factor * total;
	createEvent(m_event);
}
