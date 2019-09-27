#pragma once

#include "ecsComponentIncludes.h"

namespace ecs
{
	namespace components
	{
		struct TileComponent : public ECSComponent<TileComponent>
		{
			/*
			  This component is used to help filter for tile entities in systems.
			  It will be expanded on when pathfinding and other systems is implemented.
			*/
			int data = 0; // ELLER ENTITY ID
		};
	}
}