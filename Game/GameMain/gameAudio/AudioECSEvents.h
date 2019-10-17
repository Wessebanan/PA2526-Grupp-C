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
			AudioName audioName;
			SoundFlags soundFlags;
			// This will be implemented later
			//std::pair<SoundFlags,ID>* pInput;
		};
		struct PlayMusic : public ECSEvent<PlayMusic>
		{
			AudioName audioName;
		};
		struct FadeInMusic : public ECSEvent<FadeInMusic>
		{
			float fadeInTimeInSeconds;
		};
		struct PlaySubMusic : public ECSEvent<PlaySubMusic>
		{
			AudioName audioName;
		};
		struct FadeInSubMusic : public ECSEvent<FadeInSubMusic>
		{
			float fadeInTimeInSeconds;
		};
		struct FadeOutSubMusic : public ECSEvent<FadeOutSubMusic>
		{
			float fadeOutTimeInSeconds;
		};

	}
}