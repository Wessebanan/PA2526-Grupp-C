#pragma once

#include "ecsComponentIncludes.h"

namespace ecs
{
	namespace components
	{
		struct PositionComponent : public ECSComponent<PositionComponent>
		{
			float x = 0.0f, y = 0.0f, z = 0.0f;
		};

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