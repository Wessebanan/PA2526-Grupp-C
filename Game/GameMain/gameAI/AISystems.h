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

constexpr float DEFAULT_USAGE_OF_TILE = 0.2; //20%
constexpr float DEFAULT_TILE_SIDE_LENGTH = 0.866025403784438646763723170752f;//this value is sqrt(3)/2 that is the length to one side if the tile if radius is 1
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

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override;
			std::vector<unsigned int> GetPath(unsigned int startID, unsigned int goalID);
			int2 GetClosestTile(TransformComponent& transform);
		private:
			unsigned int FindClosestEnemy(ecs::Entity* current_unit);
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

			//Update function that prints the center position of every tile in the order they 
			//were created.
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

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override;
		private:
			//float mX;
			//float mZ;
			//float mY;
			//float mNextX;
			//float mNextZ;
			//float mYDistance;
			//float mHowMuchWeWantToUse = 0.2; //20%
			//float mTileSizeLength = sqrtf(3)/2.f;//sqrt(3)/2 is the length to one side if the tile if radius is 1
			//float mLength;
			//float mLengthOfVector;
			float mMinimumDist;
			//float mAngle;
			//XMFLOAT3 mJumpVector;
			//Returns the new state of the unit or STATE::NONE if it is supposed to stay in this state for the next update.
			STATE CheckIfGoalIsMet(FilteredEntity& entity, float delta);
			//Switch to the next units next state
			void SwitchState(FilteredEntity& entity, STATE newState);
		};

		/*
			A system that makes a unit flee from enemy units.
		*/
		class FleeStateSystem : public ECSSystem<FleeStateSystem>
		{
		public:
			FleeStateSystem();
			virtual ~FleeStateSystem();

			//Update function that prints the center position of every tile in the order they 
			//were created.
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

			//Update function that prints the center position of every tile in the order they 
			//were created.
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

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override;
		private:
			//Switch to the next units next state
			void SwitchState(FilteredEntity& entity, STATE newState);	
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

			//Update function that prints the center position of every tile in the order they 
			//were created.
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
	}
}