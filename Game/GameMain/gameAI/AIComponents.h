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
			STATE goalState = STATE::IDLE;
		};

		struct IdleStateComponent : public ECSComponent<IdleStateComponent>
		{
			int data = 0;
		};

		struct MoveStateComponent : public ECSComponent<MoveStateComponent>
		{
			STATE goalState = STATE::IDLE;
			DirectX::XMFLOAT3 goalPos = { 0.0f, 0.0f, 0.0f }; //Defaults to 0,0,0 to make sure that there always is some kind of goal.
			float time = 0.0f;
			std::vector<unsigned int> path;
			unsigned int goalID;
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
			STATE goalState = STATE::IDLE;
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