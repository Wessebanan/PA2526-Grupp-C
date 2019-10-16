#pragma once

#include <map>
#include "ecsSystemIncludes.h"
#include "../Input/InterpretWebEvents.h"
#include "AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include <iostream>
#include "../../Physics/includes/PhysicsHelperFunctions.h"

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
			TilePrintSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::TileComponent::typeID);
				typeFilter.addRequirement(components::TransformComponent::typeID);
			}
			virtual ~TilePrintSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				TransformComponent* pTc = entity.getComponent<TransformComponent>();
				std::cout << "Entity: " << pTc->getEntityID() << " Position: " << pTc->position.x << " , " << pTc->position.y << " , " << pTc->position.z << std::endl;
			}
		};

		/*
			A system that calculates the path for a unit.
		*/
		class PathfindingStateSystem : public ECSSystem<PathfindingStateSystem>
		{
		public:
			PathfindingStateSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::PathfindingStateComponent::typeID);
			}
			virtual ~PathfindingStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				/*****************************************************/
				/****************************************************/
				/* FILL OUT WITH PATHFINDING LOGIC IN ANOTHER TASK */
				/**************************************************/
				/*************************************************/

				//Add the MoveStateComponent
				ecs::components::MoveStateComponent move;
				move.goalState = entity.getComponent<PathfindingStateComponent>()->goalState;
				ecs::ECSUser::createComponent(entity.entity->getID(), move);
				//Remove the PathfindingStateComponent
				ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::PathfindingStateComponent::typeID);
			}
		};

		/*
			A system that updates idle units.
		*/
		class IdleStateSystem : public ECSSystem<IdleStateSystem>
		{
		public:
			IdleStateSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::IdleStateComponent::typeID);
				typeFilter.addRequirement(components::UnitComponent::typeID);
			}
			virtual ~IdleStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				/**********************************************************/
				/**********************************************************/
				/* FILL OUT WITH LOGIC FOR IDLE ANIMATION IN ANOTHER TASK */
				/**********************************************************/
				/**********************************************************/

				//Check if there is an enemy within fighting range.
				float attack_range = 5.0f; //SHOULD BE BASED ON EQUIPED WEAPON LATER WHEN ITEMS IS IMPLEMENTED.
				float distance = 1000.0f;
				ecs::components::UnitComponent* current_unit = entity.getComponent<ecs::components::UnitComponent>();
				ecs::components::UnitComponent* other_unit;
				ecs::components::TransformComponent* current_unit_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(current_unit->getEntityID());
				ecs::components::TransformComponent* other_unit_transform;				
				ecs::ComponentIterator it = ecs::ECSUser::getComponentsOfType<ecs::components::UnitComponent>();
				ecs::components::AttackStateComponent atk_state;
				while (other_unit = static_cast<ecs::components::UnitComponent*>(it.next()))
				{
					//Check so that the unit we are looking at isn't a friendly unit
					if (other_unit->playerID != current_unit->playerID)
					{
						other_unit_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(other_unit->getEntityID());
						distance = PhysicsHelpers::CalculateDistance(current_unit_transform->position, other_unit_transform->position);
						//Check if the enemy is within attack range.
						if (distance < attack_range)
						{
							//If the enemy is within attack range switch to a AttackStateComponent
							atk_state.enemyEntityId = other_unit->getEntityID();
							atk_state.previousState = STATE::IDLE;
							ecs::ECSUser::removeComponent(current_unit->getEntityID(), ecs::components::IdleStateComponent::typeID);
							ecs::ECSUser::createComponent(current_unit->getEntityID(), atk_state);
							break;
						}
					}
				}
			}
		};

		/*
			A system that moves a unit along a path calculated in the PathfindingSystem.
		*/
		class MoveStateSystem : public ECSSystem<MoveStateSystem>
		{
		public:
			MoveStateSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::MoveStateComponent::typeID);
				typeFilter.addRequirement(components::TransformComponent::typeID);
			}
			virtual ~MoveStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				//Fetch the move and transform component of the entity
				ecs::components::TransformComponent* transform = entity.getComponent<ecs::components::TransformComponent>();
				ecs::components::MoveStateComponent* move = entity.getComponent<ecs::components::MoveStateComponent>();

				
				/*****************************************************/
				/****************************************************/
				/*    FILL OUT WITH MOVE LOGIC IN ANOTHER TASK     */
				/**************************************************/
				/*************************************************/
				
			
				//Create the possible states we could switch to
				ecs::components::AttackStateComponent atk_state;
				atk_state.previousState = move->goalState;
				ecs::components::LootStateComponent loot_state;
				ecs::components::PathfindingStateComponent path_state;
				path_state.goalState = move->goalState;
				ecs::components::IdleStateComponent idle_state;
				//Calculate distance to goal and add the frame time to the total travel time
				float distance = PhysicsHelpers::CalculateDistance(transform->position, move->goalPos);
				float max_traveltime = 1.0f;
				move->time += delta;
				//Check if we are close enought to the goal to switch state
				if (distance < 1.0f)
				{
					ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::MoveStateComponent::typeID);
					switch (move->goalState)
					{
					case LOOT:
						ecs::ECSUser::createComponent(entity.entity->getID(), loot_state);
						break;
					case ATTACK:
						ecs::ECSUser::createComponent(entity.entity->getID(), atk_state);
						break;
					default:
						ecs::ECSUser::createComponent(entity.entity->getID(), idle_state);
						break;
					}
				}
				//Check if it is time to recalculate the path since we did it last time
				else if (move->time > max_traveltime)
				{
					ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::MoveStateComponent::typeID);
					ecs::ECSUser::createComponent(entity.entity->getID(), path_state);
				}
			}
		};

		/*
			A system that makes a unit loot an area
		*/
		class LootStateSystem : public ECSSystem<LootStateSystem>
		{
		public:
			LootStateSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::LootStateComponent::typeID);
			}
			virtual ~LootStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				/*****************************************************/
				/****************************************************/
				/*    FILL OUT WITH LOOT LOGIC IN ANOTHER TASK     */
				/**************************************************/
				/*************************************************/

				//Switch to IdleState when the area have been looted
				ecs::components::IdleStateComponent idle_state;
				ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::LootStateComponent::typeID);
				ecs::ECSUser::createComponent(entity.entity->getID(), idle_state);
			}
		};

		/*
			A system that makes a unit attack a unit
		*/
		class AttackStateSystem : public ECSSystem<AttackStateSystem>
		{
		public:
			AttackStateSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::AttackStateComponent::typeID);
			}
			virtual ~AttackStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				/*****************************************************/
				/****************************************************/
				/*   FILL OUT WITH ATTACK LOGIC IN ANOTHER TASK    */
				/**************************************************/
				/*************************************************/

				//Check if there is an enemy within fighting range.
				float attack_range = 5.0f; //SHOULD BE BASED ON EQUIPED WEAPON LATER WHEN ITEMS IS IMPLEMENTED.
				float distance = 1000.0f;
				ecs::components::UnitComponent* current_unit = entity.getComponent<ecs::components::UnitComponent>();
				ecs::components::UnitComponent* other_unit;
				ecs::components::TransformComponent* current_unit_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(current_unit->getEntityID());
				ecs::components::TransformComponent* other_unit_transform;

				//Get entity of enemy unit from our current AttackStateComponent.
				ecs::components::AttackStateComponent* atk_state = entity.getComponent<ecs::components::AttackStateComponent>();
				ecs::components::PathfindingStateComponent path_state;
				ecs::components::IdleStateComponent idle_state;
				ecs::Entity* target_entity = ecs::ECSUser::getEntity(atk_state->enemyEntityId);
				//Check if the enemy is still alive
				if (target_entity)
				{
					other_unit = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::UnitComponent>(atk_state->enemyEntityId);
					other_unit_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(atk_state->enemyEntityId);
					other_unit_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(other_unit->getEntityID());
					distance = PhysicsHelpers::CalculateDistance(current_unit_transform->position, other_unit_transform->position);
					//If the enemy is within attack range make an attack else switch to pathfinding state
					if (distance < attack_range)
					{
						/********************************************************/
						/*******************************************************/
						/*****  FILL OUT WITH ATTACK LOGIC IN ANOTHER TASK ****/
						/*****************************************************/
						/****************************************************/
					}
					else
					{
						//If the enemy is to far away: give a new pathfind component to find a route to the closest enemy.
						ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::AttackStateComponent::typeID);
						path_state.goalState = STATE::ATTACK;
						ecs::ECSUser::createComponent(entity.entity->getID(), path_state);
					}
						
				}
				else
				{
					ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::AttackStateComponent::typeID);
					//Return to idle state if the previous state was idle and the unit is dead
					if (atk_state->previousState == STATE::IDLE)
					{
						ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::AttackStateComponent::typeID);
						ecs::ECSUser::createComponent(entity.entity->getID(), idle_state);
					}
					else
					{
						//If the current command is a attack and the enemy unit died give the unit a new pathfinding component
						//to find a new target.
						ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::AttackStateComponent::typeID);
						path_state.goalState = STATE::ATTACK;
						ecs::ECSUser::createComponent(entity.entity->getID(), path_state);
					}
				}
			}
		};



		/*
			A system that reads events generated when a user sends a command with their phone.
			The system then switch the states of all units in that players army.
		*/
		class SwitchStateSystem : public ECSSystem<SwitchStateSystem>
		{
		public:
			SwitchStateSystem()
			{
				updateType = EventReader;
				typeFilter.addRequirement(ecs::events::ChangeUserStateEvent::typeID); //Needs to be added when the input event is merged into master.
			}
			virtual ~SwitchStateSystem() {}
			void readEvent(BaseEvent& event, float delta) override
			{
				//Save values for easier use.
				int player = static_cast<ecs::events::ChangeUserStateEvent*>(&event)->playerId;
				STATE state = static_cast<ecs::events::ChangeUserStateEvent*>(&event)->newState;
				//Find the correct player for the event.
				int i = 0;
				ecs::ComponentIterator it = ecs::ECSUser::getComponentsOfType(ecs::components::ArmyComponent::typeID);
				ecs::components::ArmyComponent* p_army = static_cast<ecs::components::ArmyComponent*>(it.next());
				while (i < player)
				{
					p_army = static_cast<ecs::components::ArmyComponent*>(it.next());
					i++;
				}
				//Loop through the players units and remove their old state component.
				ecs::Entity* unit;
				for (int u = 0; u < p_army->unitIDs.size(); u++)
				{
					ID entity_id = p_army->unitIDs[u];
					unit = ecs::ECSUser::getEntity(entity_id);
					
					ecs::ECSUser::removeComponent(entity_id, ecs::components::MoveStateComponent::typeID);
					ecs::ECSUser::removeComponent(entity_id, ecs::components::IdleStateComponent::typeID);
					ecs::ECSUser::removeComponent(entity_id, ecs::components::PathfindingStateComponent::typeID);
					ecs::ECSUser::removeComponent(entity_id, ecs::components::AttackStateComponent::typeID);
					ecs::ECSUser::removeComponent(entity_id, ecs::components::LootStateComponent::typeID);
					
					//Create one instance of each possible component to use in the switch case (Ugly will have to find a better way later).
					ecs::components::PathfindingStateComponent path;
					path.goalState = state;
					ecs::components::IdleStateComponent idle;
					//Give the unit the new state component.
					switch (state)
					{
					case STATE::IDLE:
						ecs::ECSUser::createComponent(entity_id, idle);
						break;
					default:
						//Defaults to a path since every other command relies on moving to a destination
						ecs::ECSUser::createComponent(entity_id, path);
						break;
					}
					/*Used for debugging*/
					//std::cout << "Changing state of player: " << player << " which has the entityID: " << p_army->getEntityID() << std::endl;
				}
			}
		};
	}
}

//////////////////////////////////////////////////////////
////Template System
////use this when creating a new system
//////////////////////////////////////////////////////////
//class testSystem : public ECSSystem<testSystem>
//{
//public:
//	testSystem()
//	{
//		updateType = EntityUpdate;
//		typeFilter.addRequirement(components::TileComponent::typeID);
//		//eventFilter.addRequirement
//		//subscribeEventCreation(
//
//		//// Entity ID -> TilePointer
//		//std::map<ID, int> tiles;
//
//		//for (int i = 0; i < 10; i++)
//		//{
//		//	// Skapar
//		//	tiles[i] = i+1;
//		//}
//		//
//		//std::map<ID, int>::iterator it;
//		//for (it = tiles.begin(); it != tiles.end(); it++)
//		//{
//		//	// it.first : ID
//		//	// it.second : int
//		//}
//	}
//	virtual ~testSystem() {}
//
//	// PICK ONE
//	//void updateEntity(FilteredEntity& entity, float delta) override;
//	//void updateMultipleEntities(EntityIterator& entities, float delta) override;
//	//void readEvent(BaseEvent& event, float delta) override;
//
//	// OPTIONAL
//	//void onEvent(TypeID eventType, BaseEvent *pEvent) override;
//};
