#pragma once

#include <string>
#include "ecsEventIncludes.h"

namespace ecs
{
	namespace events
	{
		struct TriggerSoundEvent : public ECSEvent<TriggerSoundEvent>
		{
			std::string filePath;
		};
	}
}