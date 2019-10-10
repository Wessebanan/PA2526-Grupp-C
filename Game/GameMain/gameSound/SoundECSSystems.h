#pragma once
#include "ecsSystemIncludes.h"
#include "SoundECSEvents.h"

namespace ecs
{
	namespace systems
	{
		class SoundMessageSystem : public ECSSystem<SoundMessageSystem>
		{
		public:
			SoundMessageSystem();
			virtual ~SoundMessageSystem();

			bool Init();

			void readEvent(BaseEvent& rEvent, float delta) override;
		private:
			bool SetupEngine();
			bool SetupBank();

			Sound::PaHandler* mSoundPaHandler;
			Sound::Engine* mSoundEngine;
			Sound::Mixer* mSoundMixer;
			Sound::Bank* mSoundBank;
		};
	}
}