#pragma once
#include "ecsComponentIncludes.h"

namespace ecs
{
	namespace components
	{
		// The commands in the battle phase for the users
		struct GameStateComponent : public ecs::ECSComponent<GameStateComponent>
		{
			int gameState;
		};
	}
}
