#include "SoundGlobals.h"
#include "SoundECSSystems.h"

ecs::systems::SoundMessageSystem::SoundMessageSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::PlaySound::typeID);
	typeFilter.addRequirement(ecs::events::PlayMusic::typeID);
}

ecs::systems::SoundMessageSystem::~SoundMessageSystem()
{
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
	Sound::FileData* temp_data = soundBank[p_event->soundName];
	if (temp_data == nullptr)
	{
		// No point in playing nothing
		return;
	}
	bool temp_loop_bool = p_event->soundFlags & SoundFlags::REPEAT;
	Sound::Plugin::Plugin* temp_plugin = new Sound::Plugin::Sampler(temp_data);
	soundMixer.AddSoundMessage({ temp_plugin });
}
