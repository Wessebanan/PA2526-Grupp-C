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

			void onEvent(TypeID _eventType, BaseEvent* _event) override;
		private:
			bool SetupEngine();
			bool SetupBank();

			void ProcessPlaySound(ecs::events::PlaySound* pEvent);

			void ProcessPlayMusic(ecs::events::PlayMusic* pEvent);
			void ProcessPlaySecondaryMusic(ecs::events::PlaySecondaryMusic* pEvent);
			void ProcessPlaySubMusic(ecs::events::PlaySubMusic* pEvent);

			void ProcessFadeInMusic(ecs::events::FadeInMusic* pEvent);
			void ProcessFadeInSecondaryMusic(ecs::events::FadeInSecondaryMusic* pEvent);
			void ProcessFadeInSubMusic(ecs::events::FadeInSubMusic* pEvent);

			void ProcessFadeOutMusic(ecs::events::FadeOutMusic* pEvent);
			void ProcessFadeOutSecondaryMusic(ecs::events::FadeOutSecondaryMusic* pEvent);
			void ProcessFadeOutSubMusic(ecs::events::FadeOutSubMusic* pEvent);

			void ProcessMusicSetVolume(ecs::events::MusicSetVolume* pEvent);
			void ProcessSecondaryMusicSetVolume(ecs::events::SecondaryMusicSetVolume* pEvent);
			void ProcessSubMusicSetVolume(ecs::events::SubMusicSetVolume* pEvent);

			Audio::PaHandler* mSoundPaHandler;
			Audio::Engine* mSoundEngine;
			Audio::Mixer* mSoundMixer;
			Audio::Bank* mSoundBank;
			bool mEngineInit;
		};

		class SoundCooldownClearSystem : public ECSSystem<SoundCooldownClearSystem>
		{
		public:
			SoundCooldownClearSystem();
			virtual ~SoundCooldownClearSystem();

			void updateEntity(FilteredEntity& rEntity, float delta) override;
		};
	}
}