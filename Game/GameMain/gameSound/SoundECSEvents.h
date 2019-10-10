#pragma once
#include "ecsEventIncludes.h"
#include "SoundGlobals.h"

namespace ecs
{
	namespace events
	{
		struct PlaySound : public ECSEvent<PlaySound>
		{
			SoundName soundName;
			SoundFlags soundFlags;
			// This will be implemented later
			//std::pair<SoundFlags,ID>* pInput;
		};
		struct PlayMusic : public ECSEvent<PlaySound>
		{
			SoundName soundName;
			SoundFlags soundFlags;
			// This will be implemented later
			//std::pair<SoundFlags,ID>* pInput;
		};

	}
}