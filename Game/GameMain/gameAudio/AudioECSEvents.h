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
			MusicFlags musicFlags;
			// This will be implemented later
			//std::pair<SoundFlags,ID>* pInput;
		};

	}
}