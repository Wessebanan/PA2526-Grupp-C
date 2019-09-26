#pragma once

#include "ecsComponentIncludes.h"
#include <vector>

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

		struct UnitComponent : public ECSComponent<UnitComponent>
		{
			int userID = -1;
		};

		struct ArmyComponent : public ECSComponent<ArmyComponent>
		{
			std::vector<int> unitIDs;
		};
	}
}