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
		class SoundSystem : public ecs::ECSSystem<SoundSystem>
		{
		public:
			SoundSystem();
			~SoundSystem();

			void readEvent(BaseEvent& _event, float _delta) override;
		private:
			Sound::PaHandler mPaInit;
			Sound::Engine mEngine;
			Sound::Mixer mMixer;
			Sound::Bank mBank;
		};
	}
}