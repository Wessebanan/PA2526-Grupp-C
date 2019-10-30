#pragma once

#include "ecsComponentIncludes.h"
#include <vector>
#include "..//gameSceneObjects/SceneObjectGlobals.h"
#include "../../AI/includes/AIGlobals.h"
#include <DirectXMath.h>

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
			TileType tileType = TileTypes::UNDEFINED;
			bool impassable = true;
			float niceness = 0.f;
			bool goal = false;
			unsigned int neighboursIDArray[6] = { 0 };

			BIOME biome;
		};

		struct PathfindingStateComponent : public ECSComponent<PathfindingStateComponent>
		{
			STATE activeCommand = STATE::IDLE;
		};

		struct IdleStateComponent : public ECSComponent<IdleStateComponent>
		{
			int data = 0;
		};

		struct MoveStateComponent : public ECSComponent<MoveStateComponent>
		{
			STATE activeCommand = STATE::IDLE;
			unsigned int goalID;
			float time = 0.0f; 
			std::vector<unsigned int> path;
		};

		struct FleeStateComponent : public ECSComponent<FleeStateComponent>
		{
			/*FILLED OUT WITH DATA IN ANOTHER TASK.*/
			int data = 0;
		};

		struct LootStateComponent : public ECSComponent<LootStateComponent>
		{
			int data = 0;
			std::vector<unsigned int> path;
		};

		struct AttackStateComponent : public ECSComponent<AttackStateComponent>
		{
			int enemyEntityId = 0;
			STATE activeCommand = STATE::IDLE;
		};

		struct UnitComponent : public ECSComponent<UnitComponent>
		{
			PLAYER playerID; //Holds an enum to know which player the unit belongs to.
		};

		struct ArmyComponent : public ECSComponent<ArmyComponent>
		{

			std::vector<int> unitIDs; //Holds the entity IDs of every unit in one players army.

			PLAYER playerID;
		};

		struct DeadComponent : public ECSComponent<DeadComponent>
		{
			int data;
		};
	}
}