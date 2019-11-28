#pragma once
#include "ecsEventIncludes.h"
#include "AudioGlobals.h"
#undef PlaySound	// Some evil macro conflicting with my name
namespace ecs
{
	namespace events
	{
		struct PlaySound : public ECSEvent<PlaySound>
		{
			AudioName audioName = (AudioName)0;
			SoundFlags soundFlags = (SoundFlags)0;
			ID invokerEntityId = 0;
		};

		struct PlayMusic : public ECSEvent<PlayMusic>
		{
			AudioName audioName;
		};
		struct PlaySecondaryMusic : public ECSEvent<PlaySecondaryMusic>
		{
			AudioName audioName;
		};
		struct PlaySubMusic : public ECSEvent<PlaySubMusic>
		{
			AudioName audioName;
		};

		struct FadeInMusic : public ECSEvent<FadeInMusic>
		{
			float fadeInTimeInSeconds;
		};
		struct FadeInSecondaryMusic : public ECSEvent<FadeInSecondaryMusic>
		{
			float fadeInTimeInSeconds;
		};
		struct FadeInSubMusic : public ECSEvent<FadeInSubMusic>
		{
			float fadeInTimeInSeconds;
		};

		struct FadeOutMusic : public ECSEvent<FadeOutMusic>
		{
			float fadeOutTimeInSeconds;
		};
		struct FadeOutSecondaryMusic : public ECSEvent<FadeOutSecondaryMusic>
		{
			float fadeOutTimeInSeconds;
		};
		struct FadeOutSubMusic : public ECSEvent<FadeOutSubMusic>
		{
			float fadeOutTimeInSeconds;
		};

		struct MusicSetVolume : public ECSEvent<MusicSetVolume>
		{
			float volume;
		};
		struct SecondaryMusicSetVolume : public ECSEvent<MusicSetVolume>
		{
			float volume;
		};
		struct SubMusicSetVolume : public ECSEvent<SubMusicSetVolume>
		{
			float volume;
		};

	}
}