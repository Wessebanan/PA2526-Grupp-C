#pragma once

#include "ecsComponentIncludes.h"
#include "AIGlobals.h"

namespace ecs
{
	namespace components
	{
		struct TileComponent : public ECSComponent<TileComponent> //Component used for AI pathfinding
		{
			/*
			  This component is used to help filter for tile entities in systems.
			  It will be expanded on when pathfinding and other systems is implemented.
			*/
			TileType tileType = Grass;
			bool impassable = false;
		};
	}
}