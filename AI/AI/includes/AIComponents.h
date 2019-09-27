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

		struct PathfindingStateComponent : public ECSComponent<PathfindingStateComponent>
		{
			int data = 0;
		};

		struct IdleStateComponent : public ECSComponent<IdleStateComponent>
		{
			int data = 0;
		};

		struct MoveStateComponent : public ECSComponent<MoveStateComponent>
		{
			int data = 0;
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