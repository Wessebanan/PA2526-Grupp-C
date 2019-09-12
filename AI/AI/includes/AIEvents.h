#pragma once

#include "ecsEventIncludes.h"

namespace ecs
{
	namespace events
	{
		struct testEvent : public ECSEvent<testEvent>
		{
			// data
		};
	}
}