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
				typeFilter.addRequirement(components::TransformComponent::typeID);
			}
			virtual ~PathfindingStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				ecs::components::PathfindingStateComponent* path_comp = entity.getComponent<ecs::components::PathfindingStateComponent>();
				int2 start_tile = 0;
				std::vector<unsigned int> path;
				GridProp* g_prop = GridProp::GetInstance();
				components::TransformComponent* p_transfrom = entity.getComponent<TransformComponent>();
				ecs::components::TransformComponent* goal_transform;
				start_tile = GridFunctions::GetTileFromWorldPos(p_transfrom->position.x, p_transfrom->position.z);
				unsigned int start_tile_id = g_prop->mGrid[start_tile.y][start_tile.x].Id;
				unsigned int goal_id = 0;
				unsigned int goal_tile_id = 0;
				int2 goal_tile_index;
				if (path_comp->goalState == STATE::ATTACK)
				{
					goal_id = this->FindClosestEnemy(entity.entity);
					goal_transform = static_cast<ecs::components::TransformComponent*>(ecs::ECSUser::getComponentFromKnownEntity(ecs::components::TransformComponent::typeID, goal_id));
					goal_tile_index = GridFunctions::GetTileFromWorldPos(goal_transform->position.x, goal_transform->position.z);
					if (goal_id = g_prop->mGrid[goal_tile_index.y][goal_tile_index.x].isPassable)
					{
						goal_id = g_prop->mGrid[goal_tile_index.y][goal_tile_index.x].Id;
					}
					
				}
				else if (path_comp->goalState == STATE::LOOT)
				{
					goal_id = 117;
				}
				if(g_prop->mGrid[start_tile.y][start_tile.x].isPassable)
				{
					path = GetPath(start_tile_id, goal_id);
				}

				components::MoveStateComponent move_comp;
				move_comp.path = path;
				move_comp.goalID = goal_id;
				move_comp.goalState = entity.getComponent<PathfindingStateComponent>()->goalState;
				ecs::ECSUser::createComponent(entity.entity->getID(), move_comp);
				//Remove the PathfindingStateComponent
				ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::PathfindingStateComponent::typeID);
				
			}
			std::vector<unsigned int> GetPath(unsigned int startID, unsigned int goalID)
			{
				struct NodeInfo
				{
					float move_cost;
					unsigned int id;
					unsigned int parent_id;
				};
				std::vector<NodeInfo> open_list; //open list for all the potential new tiles to visit 
				std::vector<unsigned int> to_return;
				std::unordered_map<unsigned int, NodeInfo> closed_list; //key is its own ID and data is nodeinfo struct, used for tiles that have been visited
				components::TileComponent* current_tile = nullptr;
				components::TileComponent* current_neighbour_tile = nullptr;
				components::TransformComponent* current_neighbour_transfrom = nullptr;
				components::TransformComponent* goal_tile_transfrom = nullptr;

				float cost = 999.f;
				bool in_open_list = false;
				float dist_to_goal = 0.f;
				int pos_in_open_list = 0;
				unsigned int parent_id = 0;
				unsigned int next_tile_id = 0;
				float best_neighbour_cost = 999.f;
				unsigned int current_tile_id = startID;

				current_tile = ecs::ECSUser::getComponentFromKnownEntity<components::TileComponent>(startID);
				goal_tile_transfrom = ecs::ECSUser::getComponentFromKnownEntity<components::TransformComponent>(goalID);

				//Add the start tile to the closed list
				NodeInfo start_node;
				start_node.id = startID;
				start_node.parent_id = startID;
				start_node.move_cost = 0;
				closed_list[current_tile_id] = start_node;
				if(current_tile != nullptr && goal_tile_transfrom != nullptr) // maybe remove later because they should never be sent to pathfinding of they are null
				{
					while (current_tile_id != goalID) 
					{
						for (int i = 0; i < 6; i++) // put new neighbours in open list or maybe update old neighbour move cost
						{
							if (current_tile->neighboursIDArray[i] != 0 &&
								!closed_list.count(current_tile->neighboursIDArray[i])) //check so that the neightbour is valid and not in the closed list
							{
								pos_in_open_list = 0;
								in_open_list = false;
								for (NodeInfo j : open_list) // check if the neighbour is in the open list
								{
									if (j.id == current_tile->neighboursIDArray[i])
									{
										in_open_list = true;
										break;
									}
									pos_in_open_list++;
								}
								if (in_open_list) //if the neighbour is in the open list we might update its move_cost if the cost is lower this time
								{
									current_neighbour_transfrom = ecs::ECSUser::getComponentFromKnownEntity<components::TransformComponent>(current_tile->neighboursIDArray[i]);
									current_neighbour_tile = ecs::ECSUser::getComponentFromKnownEntity<components::TileComponent>(current_tile->neighboursIDArray[i]);
									dist_to_goal = GridFunctions::GetDistance(current_neighbour_transfrom->position.x, current_neighbour_transfrom->position.z, //calc neighbour dist from goal
										goal_tile_transfrom->position.x, goal_tile_transfrom->position.z);
									cost = current_neighbour_tile->niceness + dist_to_goal + closed_list[current_tile_id].move_cost; //calc cost for move
									if (cost < open_list.at(pos_in_open_list).move_cost) // if move cost is better then last time, update it
									{
										open_list.at(pos_in_open_list).move_cost = cost;
									}
								}
								else if (!in_open_list) //if the neighbour is valid and not in the closed or open list we add it to the open list
								{
									current_neighbour_transfrom = ecs::ECSUser::getComponentFromKnownEntity<components::TransformComponent>(current_tile->neighboursIDArray[i]);
									current_neighbour_tile = ecs::ECSUser::getComponentFromKnownEntity<components::TileComponent>(current_tile->neighboursIDArray[i]);
									dist_to_goal = GridFunctions::GetDistance(current_neighbour_transfrom->position.x, current_neighbour_transfrom->position.z,
										goal_tile_transfrom->position.x, goal_tile_transfrom->position.z);
									cost = current_neighbour_tile->niceness + dist_to_goal + closed_list[current_tile_id].move_cost; //calc cost for move

									NodeInfo to_insert;
									to_insert.id = current_tile->neighboursIDArray[i]; //give it its id
									to_insert.parent_id = current_tile_id;			  //give it its parents id
									to_insert.move_cost = cost;						 //give it the move_cost it would be if we were to move to it
									open_list.push_back(to_insert);					//insert new tile into openlist
								}
							}
						}
						
						pos_in_open_list = 0;
						best_neighbour_cost = 999;
						//we go through all the potential tiles we can go to and find the one with the lowest cost to move to, this usually means it is closer to the goal
						for (int i = 0; i < open_list.size(); i++) 
						{
							if (open_list.at(i).move_cost < best_neighbour_cost)
							{
								best_neighbour_cost = open_list.at(i).move_cost;
								next_tile_id = open_list.at(i).id;
								parent_id = open_list.at(i).parent_id;
								pos_in_open_list = i;
							}
						}
						//when we have found the best next move we add it to the closed list and remove it from the open list and then make this new tile our current tile
						closed_list[next_tile_id].id = next_tile_id;				//its own id
						closed_list[next_tile_id].parent_id = parent_id;		   //parent id
						closed_list[next_tile_id].move_cost = best_neighbour_cost;//its movecost
						if(open_list.size() > 0 )
						{
							open_list.erase(open_list.begin() + pos_in_open_list);	//remove from openlist
						}
						current_tile = ecs::ECSUser::getComponentFromKnownEntity<components::TileComponent>(next_tile_id);
						current_tile_id = next_tile_id;
						
					}
					//when we have found the goal we start to build the path by starting with the goal tile then we add that tiles parent tile and do this until we come to the start tile
					//parent tile in this function is the tile that was used to get to the child tile, so when we find the goal we just the go backwards using the parents
					current_tile_id = goalID;
					to_return.push_back(current_tile_id);
					while (current_tile_id != startID)
					{ 
						current_tile_id = closed_list[current_tile_id].parent_id;
						to_return.push_back(current_tile_id);
					}
					return to_return;
				}
				else
				{
					to_return.push_back(startID);
					return to_return;
				}
			}
		private:
			unsigned int FindClosestEnemy(ecs::Entity* current_unit)
			{
				ecs::ComponentIterator ittr;
				ecs::BaseComponent* p_base_component;
				ecs::components::ArmyComponent* army_comp;
				ecs::Entity* other_unit;
				ecs::components::UnitComponent* curr_unit_comp = static_cast<ecs::components::UnitComponent*>(ecs::ECSUser::getComponentFromKnownEntity(ecs::components::UnitComponent::typeID, current_unit->getID()));
				ecs::components::UnitComponent* other_unit_comp;
				ecs::components::TransformComponent* curr_unit_transform = static_cast<ecs::components::TransformComponent*>(ecs::ECSUser::getComponentFromKnownEntity(ecs::components::TransformComponent::typeID, current_unit->getID()));
				ecs::components::TransformComponent* other_unit_transform;
				float dist = 1000.0f;
				float temp_dist = 0.0f;
				unsigned int enemy_id;

				ittr = ecs::ECSUser::getComponentsOfType(ecs::components::ArmyComponent::typeID);
				while (p_base_component = ittr.next())
				{
					army_comp = static_cast<ecs::components::ArmyComponent*>(p_base_component);
					for (int i = 0; i < army_comp->unitIDs.size(); i++)
					{
						other_unit = ecs::ECSUser::getEntity(army_comp->unitIDs[i]);
						other_unit_comp = static_cast<ecs::components::UnitComponent*>(ecs::ECSUser::getComponentFromKnownEntity(ecs::components::UnitComponent::typeID, other_unit->getID()));
						if (other_unit_comp->playerID != curr_unit_comp->playerID)
						{
							other_unit_transform = static_cast<ecs::components::TransformComponent*>(ecs::ECSUser::getComponentFromKnownEntity(ecs::components::TransformComponent::typeID, other_unit->getID()));
							temp_dist = PhysicsHelpers::CalculateDistance(curr_unit_transform->position, other_unit_transform->position);
							if (temp_dist < dist)
							{
								dist = temp_dist;
								enemy_id = other_unit->getID();
							}
						}
					}
				}
				return enemy_id;
			}
				/*****************************************************/
				/****************************************************/
				/* FILL OUT WITH PATHFINDING LOGIC IN ANOTHER TASK */
				/**************************************************/
				/*************************************************/

				
			
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
				float attack_range = 0.0f; //SHOULD BE BASED ON EQUIPED WEAPON LATER WHEN ITEMS IS IMPLEMENTED.
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
				typeFilter.addRequirement(components::DynamicMovementComponent::typeID);
			}
			virtual ~MoveStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				//Fetch the move and transform component of the entity
				ecs::components::TransformComponent* transform = entity.getComponent<ecs::components::TransformComponent>();

				ecs::components::DynamicMovementComponent* dyn_move = entity.getComponent<ecs::components::DynamicMovementComponent>();
				ecs::components::MoveStateComponent* move_comp = entity.getComponent<ecs::components::MoveStateComponent>();
				if (move_comp->path.size() > 0)
				{
					ecs::components::TransformComponent* goal = getComponentFromKnownEntity<components::TransformComponent>(move_comp->path.back());
					if(goal != nullptr)
					{
						if (abs(goal->position.x - transform->position.x) < 1.f && abs(goal->position.z - transform->position.z) < 1.f)
						{
							move_comp->path.pop_back();
						}
						if (move_comp->path.size() > 0)
						{
							goal = getComponentFromKnownEntity<components::TransformComponent>(move_comp->path.back());
							this->x = goal->position.x - transform->position.x;
							this->z = goal->position.z - transform->position.z;
							this->length = sqrt(x * x + z * z);
							this->x = this->x / this->length;
							this->z = this->z / this->length;
							dyn_move->mForward.x = this->x;
							dyn_move->mForward.z = this->z;

							MovementInputEvent kek;
							kek.mInput = FORWARD;
							kek.mEntityID = entity.entity->getID();
							createEvent(kek);//creates an event to physics to move character
					}

						//Create the possible states we could switch to
						ecs::components::AttackStateComponent atk_state;
						atk_state.previousState = move_comp->goalState;
						atk_state.enemyEntityId = move_comp->goalID;
						ecs::components::LootStateComponent loot_state;
						ecs::components::PathfindingStateComponent path_state;
						path_state.goalState = move_comp->goalState;
						ecs::components::IdleStateComponent idle_state;
						//Calculate distance to goal and add the frame time to the total travel time
						float distance = PhysicsHelpers::CalculateDistance(transform->position, move_comp->goalPos);
						float max_traveltime = 1.f;
						move_comp->time += delta;
						//Check if we are close enought to the goal to switch state
						if (distance < 0.1f)
						{
							ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::MoveStateComponent::typeID);
							switch (move_comp->goalState)
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
						else if (move_comp->time > max_traveltime)
						{
							ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::MoveStateComponent::typeID);
							ecs::ECSUser::createComponent(entity.entity->getID(), path_state);
						}
					}
				}
			}
		private:
			float x;
			float z;
			float length;
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
				typeFilter.addRequirement(components::UnitComponent::typeID);
				typeFilter.addRequirement(components::TransformComponent::typeID);
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
				float attack_range = 0.01f; //SHOULD BE BASED ON EQUIPED WEAPON LATER WHEN ITEMS IS IMPLEMENTED.
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
				typeFilter.addRequirement(ecs::events::ChangeUserStateEvent::typeID);
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