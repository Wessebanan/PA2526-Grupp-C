#pragma once

#include "ecsComponentIncludes.h"
#include <vector>
#include "..//gameSceneObjects/SceneObjectGlobals.h"
#include "../../AI/includes/AIGlobals.h"
#include <DirectXMath.h>

struct PotentialFieldCharges
{
	float armyCharges[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	float hazardCharge = 0.0f;
};

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
			float niceness = 0.f;
			bool impassable = true;
			bool goal = false;
			bool hasTrap = false;
			unsigned int neighboursIDArray[6] = { 0 };
			PotentialFieldCharges charges;

			BIOME biome;
		};

		struct PathfindingStateComponent : public ECSComponent<PathfindingStateComponent>
		{
			STATE activeCommand = STATE::IDLE;
		};

		struct IdleStateComponent : public ECSComponent<IdleStateComponent>
		{
			STATE activeCommand = STATE::IDLE;
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
			STATE activeCommand = STATE::IDLE;
		};

		struct LootStateComponent : public ECSComponent<LootStateComponent>
		{
			unsigned int goalID;
		};

		struct RallyStateComponent : public ECSComponent<RallyStateComponent>
		{

		};

		struct AttackStateComponent : public ECSComponent<AttackStateComponent>
		{
			int enemyEntityId = 0;
			STATE activeCommand = STATE::IDLE;
		};

		struct UnitComponent : public ECSComponent<UnitComponent>
		{
			PLAYER playerID; //Holds an enum to know which player the unit belongs to.
			float length = 0;
			float timeSinceStuck = 0.0f;
			DirectX::XMFLOAT3 lastPos;
			bool hasDiedBefore = false;
		};

		struct ArmyComponent : public ECSComponent<ArmyComponent>
		{

			std::vector<int> unitIDs; //Holds the entity IDs of every unit in one players army.
			
			PLAYER playerID;
		};

		struct DeadComponent : public ECSComponent<DeadComponent>
		{
			enum CauseOfDeath
			{
				CAUSE_DROWNING,
				CAUSE_DAMAGE
			};
			CauseOfDeath cause;			// Kind of death
			DirectX::XMFLOAT3 position;	// Save position of where the dead happened
		};

		struct UnitScalePercent : public ECSComponent<UnitScalePercent>
		{
			float UnitScale = 1.f; //Holds an enum to know which player the unit belongs to.
		};

		struct PlayerStateComponent : public ECSComponent<PlayerStateComponent>
		{
			STATE mCurrentStates[4];
		};

		struct AiBrainComponent : public ECSComponent<AiBrainComponent>
		{
			PLAYER mPlayer;
			float mTimer = 0.0f;
		};
		
		struct JumpComponent : public ECSComponent<JumpComponent>
		{

		};
	}
}