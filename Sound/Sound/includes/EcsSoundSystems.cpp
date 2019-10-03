#include "EcsSoundSystems.h"

ecs::systems::SoundSystem::SoundSystem()
{
	updateType = SystemUpdateType::EventReader;
	typeFilter.addRequirement(events::CreateComponentEvent::typeID);

	mEngine.UseThisMixer(&mMixer);
	mEngine.OpenStream();
	mEngine.StartStream();
	mEngine.StartWorkThread();
}

ecs::systems::SoundSystem::~SoundSystem()
{
	mEngine.JoinWorkThread();
	mEngine.StopStream();
	mEngine.CloseStream();
}

void ecs::systems::SoundSystem::readEvent(BaseEvent& _event, float _delta)
{
	events::TriggerSoundEvent* sound_event = static_cast<events::TriggerSoundEvent*>(&_event);

	//sound_event->filePath;
	Sound::FileData* file_data = mBank.GetFile(sound_event->filePath);
	if (file_data == nullptr)
	{
		return;
	}
	mMixer.NewVoice(new Sound::Plugin::Sampler(file_data));
	//ECSUser::createEvent();
}
