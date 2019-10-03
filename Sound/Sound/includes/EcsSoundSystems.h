#pragma once

#include "ecsSystemIncludes.h"

#include "EcsSoundComponents.h"
#include "EcsSoundEvents.h"
#include "SoundEngine.h"
#include "Mixer.h"
#include "SoundBank.h"
#include "Plugin.h"

namespace ecs
{
	namespace systems
	{
		class SoundSystem : ecs::ECSSystem<SoundSystem>
		{
		public:
			SoundSystem()
			{
				updateType = SystemUpdateType::EventReader;
				typeFilter.addRequirement(events::CreateComponentEvent::typeID);
				
				mEngine.UseThisMixer(&mMixer);
				mEngine.OpenStream();
				mEngine.StartStream();
				mEngine.StartWorkThread();
			}
			~SoundSystem()
			{
				mEngine.JoinWorkThread();
				mEngine.StopStream();
				mEngine.CloseStream();
			}

			void readEvent(BaseEvent& _event, float _delta) override
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


		private:
			Sound::PaHandler mPaInit;
			Sound::Engine mEngine;
			Sound::Mixer mMixer;
			Sound::Bank mBank;
		};
	}
}