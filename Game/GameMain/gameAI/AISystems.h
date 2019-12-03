#pragma once

#include <map>
#include "ecsSystemIncludes.h"
#include "../Input/InterpretWebEvents.h"
#include "AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "..//..//AI/includes/GridFunctions.h"
#include "..//..//AI/includes/GridProp.h"
#include "../Physics/PhysicsComponents.h"
#include "../Physics/PhysicsEvents.h"
#include <iostream>
#include "../../Physics/includes/PhysicsHelperFunctions.h"
#include "../gameAnimation/AnimationComponents.h"
#include "../UI/UIComponents.h"
#include "../../AI/includes/AIGlobals.h"
#include "../GameGlobals.h"
#include "../gameGraphics/ParticleECSComponents.h"	
#include "../gameAudio/AudioECSEvents.h"


constexpr float DEFAULT_USAGE_OF_TILE = 0.2; //20%
constexpr float DEFAULT_TILE_SIDE_LENGTH = 0.866025403784438646763723170752f;//this value is sqrt(3)/2 that is the length to one side if the tile if radius is 1
constexpr float SIZE_REWARD		= 1.1f;
constexpr float HEALTH_REWARD	= 0.2f;
constexpr float ATTACK_REWARD	= 1.1f;
namespace ecs
{
	namespace systems
	{
		/*
			A system that prints every tiles centerposition in the order it was
			created. Only used for debugging purposes.
		*/
		class TilePrintSystem : public ECSSystem<TilePrintSystem>
		{
		public:
			TilePrintSystem();
			virtual ~TilePrintSystem();

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		/*
			A system that calculates the path for a unit.
		*/
		class PathfindingStateSystem : public ECSSystem<PathfindingStateSystem>
		{
		public:
			PathfindingStateSystem();
			virtual ~PathfindingStateSystem();

			//Update function that calculates the path for the given unit based on the command given.
			void updateEntity(FilteredEntity& entity, float delta) override;
			std::vector<unsigned int> GetPath(unsigned int startID, unsigned int goalID);
			int2 GetClosestTile(TransformComponent& transform);
		private:
			unsigned int FindClosestEnemy(ecs::Entity* current_unit);
			unsigned int FindClosestFriendWithoutWeapon(ecs::Entity* current_unit);
			unsigned int FindClosestFriend(ecs::Entity* current_unit);
			unsigned int FindSafeTile(ecs::Entity* current_unit);
			unsigned int FindClosestLootTile(ecs::Entity* current_unit);
		};

		/*
			A system that updates idle units. Will attack enemy units if they come to close.
		*/
		class IdleStateSystem : public ECSSystem<IdleStateSystem>
		{
		public:
			IdleStateSystem();
			virtual ~IdleStateSystem();

			//Update function that makes the unit idle and if an enemy unit moves to close
			//the unit will either attack or run away depending on what command it currently follows.
			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		/*
		A system that moves a unit along a path calculated in the PathfindingSystem.
		*/
		class MoveStateSystem : public ECSSystem<MoveStateSystem>
		{
		public:
			MoveStateSystem();
			virtual ~MoveStateSystem();

			//Update function that moves the unit along its calculated path from the Pathfindingsystem
			void updateEntity(FilteredEntity& entity, float delta) override;
		private:
			float mMinimumDist;
			//Returns the new state of the unit or STATE::NONE if it is supposed to stay in this state for the next update.
			STATE CheckIfGoalIsMet(FilteredEntity& entity, float delta);
			//Switch to the next units next state
			void SwitchState(FilteredEntity& entity, STATE newState);
			ID GetClosestTileId(TransformComponent& transform);
		};

		/*
			A system that makes a unit flee from enemy units.
		*/
		class FleeStateSystem : public ECSSystem<FleeStateSystem>
		{
		public:
			FleeStateSystem();
			virtual ~FleeStateSystem();

			//Update function that checks if all enemy units is far enough away: if so the unit goes idle.
			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		/*
			A system that makes a unit loot an area
		*/
		class LootStateSystem : public ECSSystem<LootStateSystem>
		{
		public:
			LootStateSystem();
			virtual ~LootStateSystem();

			//Update function that doesn't do anything atm. Might be used in the future to start sounds or an animation.
			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		/*
			A system that makes a unit attack a unit
		*/
		class AttackStateSystem : public ECSSystem<AttackStateSystem>
		{
		public:
			AttackStateSystem();
			virtual ~AttackStateSystem();

			//Update function that checks so that the units target is still alive and within attack range otherwise
			//it makes the unit path to a new enemy target.
			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		/*
			A system that removes units with the DeadComponent at the end of each frame. Also removes the weapon of that unit if it is a FIST weapon
			else it sets the weapons owner to 0 so that another unit can pick it up.
		*/
		class RemoveDeadUnitsSystem : public ECSSystem<RemoveDeadUnitsSystem>
		{
		public:
			RemoveDeadUnitsSystem();
			virtual ~RemoveDeadUnitsSystem();

			//Update function that removes units with the DeadComponent
			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		/*
			A system that reads events generated when a user sends a command with their phone.
			The system then switch the states of all units in that players army.
		*/
		class SwitchStateSystem : public ECSSystem<SwitchStateSystem>
		{
		public:
			SwitchStateSystem();
			virtual ~SwitchStateSystem();
			void readEvent(BaseEvent& event, float delta) override;
		};

		/*
			A system that calculate the water hazard effect on each tile on the map. This system is only supposed
			to be updated once when a map has been created since the water tiles won't change during the game.
		*/
		class PotentialWaterHazardSystem : public ECSSystem<PotentialWaterHazardSystem>
		{
		public:
			PotentialWaterHazardSystem();
			virtual ~PotentialWaterHazardSystem();

			//Update function that calculates the water tiles hazard influence on each tile.
			void updateMultipleEntities(EntityIterator& entities, float delta) override;
		};

		/*
			A system that calculate the each armies hazards on each tile in the world that is not a water tile. This
			system should be run at the begining of each frame.
		*/
		class PotentialArmyHazardSystem : public ECSSystem<PotentialArmyHazardSystem>
		{
		public:
			PotentialArmyHazardSystem();
			virtual ~PotentialArmyHazardSystem();

			//Update function that calculates the armies hazard influence on each tile.
			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		/*
			A system that creates a random command for a CPU player in a given time interval.
		*/
		class AIPlayerSystem : public ECSSystem<AIPlayerSystem>
		{
		public:
			AIPlayerSystem();
			virtual ~AIPlayerSystem();

			//Update function that calculates the armies hazard influence on each tile.
			void updateEntity(FilteredEntity& entity, float delta) override;
		};
	}
}