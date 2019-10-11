#pragma once

#include "ecsComponentIncludes.h"
#include <vector>
#include "../../AI/includes/AIGlobals.h"

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
			TileType tileType = GRASS;
			bool impassable = true;
			float niceness = 0.f;
			bool goal = false;
			unsigned int neighboursIDArray[6] = { 0 };
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
			std::vector<unsigned int> path;
		};

		struct UnitComponent : public ECSComponent<UnitComponent>
		{
			PLAYER playerID; //Holds an enum to know which player the unit belongs to.
		};

		struct ArmyComponent : public ECSComponent<ArmyComponent>
		{
			std::vector<int> unitIDs; //Holds the entity IDs of every unit in one players army.
		};
	}
}