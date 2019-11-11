#pragma once

#include "ecsComponentIncludes.h"
#include "../GameGlobals.h"

namespace ecs
{
	namespace components
	{
		struct TrapComponent : public ECSComponent<TrapComponent>
		{
			GAME_OBJECT_TYPE mObjectType;
		};
	}
}