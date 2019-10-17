#pragma once
#include "ecsSystemIncludes.h"
#include "AudioECSEvents.h"

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

			void ProcessPlaySound(ecs::events::PlaySound* pEvent);
			void ProcessPlayMusic(ecs::events::PlayMusic* pEvent);
			void ProcessFadeInMusic(ecs::events::FadeInMusic* pEvent);
			void ProcessPlaySubMusic(ecs::events::PlaySubMusic* pEvent);
			void ProcessFadeInSubMusic(ecs::events::FadeInSubMusic* pEvent);
			void ProcessFadeOutSubMusic(ecs::events::FadeOutSubMusic* pEvent);

			Audio::PaHandler* mSoundPaHandler;
			Audio::Engine* mSoundEngine;
			Audio::Mixer* mSoundMixer;
			Audio::Bank* mSoundBank;
		};
	}
}