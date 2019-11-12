#pragma once

#include "ecsEventIncludes.h"

namespace ecs
{
	namespace events
	{
		struct CountdownStartEvent : public ecs::ECSEvent<CountdownStartEvent>
		{};
	}
}