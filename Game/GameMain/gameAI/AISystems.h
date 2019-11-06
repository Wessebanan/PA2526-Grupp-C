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
				ecs::components::TransformComponent* goal_transform = nullptr;
				//start_tile = GridFunctions::GetTileFromWorldPos(p_transfrom->position.x, p_transfrom->position.z);
				start_tile = this->GetClosestTile(*p_transfrom);
				unsigned int start_tile_id = g_prop->mGrid[start_tile.y][start_tile.x].Id;
				unsigned int goal_id = 0;
				unsigned int goal_enemy_id = 0;
				unsigned int goal_friend_id = 0;
				unsigned int goal_tile_id = 0;
				int2 goal_tile_index;
				bool calc_path = false;
				bool already_have_weapon = false;

				//Find the goal we want to move to depending on which command the unit was given.
				switch (path_comp->activeCommand)
				{
					case STATE::ATTACK:
					{
						//Find the closest enemy.
						goal_enemy_id = this->FindClosestEnemy(entity.entity);
						if (goal_enemy_id != 0)
						{
							//Find the closest tile to the nearest enemy.
							goal_transform = static_cast<ecs::components::TransformComponent*>(ecs::ECSUser::getComponentFromKnownEntity(ecs::components::TransformComponent::typeID, goal_enemy_id));
							//goal_tile_index = GridFunctions::GetTileFromWorldPos(goal_transform->position.x, goal_transform->position.z);
							goal_tile_index = this->GetClosestTile(*goal_transform);
							//Check that the tile is traversable
							if (goal_id = g_prop->mGrid[goal_tile_index.y][goal_tile_index.x].isPassable)
							{
								goal_id = g_prop->mGrid[goal_tile_index.y][goal_tile_index.x].Id;
							}
							calc_path = true;
						}
						break;
					}
					case STATE::LOOT:
					{
						ecs::components::EquipmentComponent* equipment_comp = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::EquipmentComponent>(entity.entity->getID());
						ecs::components::WeaponComponent* weapon_comp = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::WeaponComponent>(equipment_comp->mEquippedWeapon);
						if (weapon_comp->mType != GAME_OBJECT_TYPE_FIST)
						{
							goal_friend_id = this->FindClosestFriend(entity.entity);
							if (goal_friend_id != 0)
							{
								//Find the closest tile to the nearest friend.
								goal_transform = static_cast<ecs::components::TransformComponent*>(ecs::ECSUser::getComponentFromKnownEntity(ecs::components::TransformComponent::typeID, goal_friend_id));
								//goal_tile_index = GridFunctions::GetTileFromWorldPos(goal_transform->position.x, goal_transform->position.z);
								goal_tile_index = this->GetClosestTile(*goal_transform);
								//Check that the tile is traversable
								if (goal_id = g_prop->mGrid[goal_tile_index.y][goal_tile_index.x].isPassable)
								{
									goal_id = g_prop->mGrid[goal_tile_index.y][goal_tile_index.x].Id;
								}
								calc_path = true;
								already_have_weapon = true;
							}
						}
						else
						{
							goal_id = this->FindClosestLootTile(entity.entity);
						}
						if (goal_id != 0)
						{
							calc_path = true;
						}
						break;
					}
					case STATE::FLEE:
					{
						//Find the safest tile in the arena
						goal_id = FindSafeTile(entity.entity);
						calc_path = true;
						break;
					}
					default:
					{
						break;
					}
				}
				//Calculate the path to the goal if a valid goal was found.
				if (calc_path)
				{
					if (g_prop->mGrid[start_tile.y][start_tile.x].isPassable)
					{
						path = GetPath(start_tile_id, goal_id);
					}
					//Create the move component and set its variables.
					components::MoveStateComponent move_comp;
					move_comp.path = path;
					move_comp.activeCommand = entity.getComponent<PathfindingStateComponent>()->activeCommand;

					//If the command is to attack we want to save the nearest enemies id is as the goal id instead of the nearest tile.
					if (move_comp.activeCommand == ATTACK) 
					{
						move_comp.goalID = goal_enemy_id;
					}
					else if (move_comp.activeCommand == LOOT && already_have_weapon)
					{
						move_comp.goalID = goal_friend_id;
					}
					else
					{
						move_comp.goalID = goal_id;
					}
					//Create the move component and hand it to the unit.
					ecs::ECSUser::createComponent(entity.entity->getID(), move_comp);
				}
				else
				{
					//If no valid goal was found we set the unit to idle.
					components::IdleStateComponent idle_comp;
					ecs::ECSUser::createComponent(entity.entity->getID(), idle_comp);
				}
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
				bool no_path_found = false;

				current_tile = ecs::ECSUser::getComponentFromKnownEntity<components::TileComponent>(startID);
				goal_tile_transfrom = ecs::ECSUser::getComponentFromKnownEntity<components::TransformComponent>(goalID);

				GridProp* p_gp = GridProp::GetInstance();

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
						if (open_list.size() == 0)//if no path was found switch the bool and break the loop
						{
							no_path_found = true;
							break;
						}
						if(open_list.size() > 0 )
						{
							open_list.erase(open_list.begin() + pos_in_open_list);	//remove from openlist
						}
						current_tile = ecs::ECSUser::getComponentFromKnownEntity<components::TileComponent>(next_tile_id);
						current_tile_id = next_tile_id;
						
					}
					//when we have found the goal we start to build the path by starting with the goal tile then we add that tiles parent tile and do this until we come to the start tile
					//parent tile in this function is the tile that was used to get to the child tile, so when we find the goal we just the go backwards using the parents
					if(!no_path_found) //if path was found
					{
						current_tile_id = goalID;
						to_return.push_back(current_tile_id);
						while (current_tile_id != startID)
						{ 
							current_tile_id = closed_list[current_tile_id].parent_id;
							to_return.push_back(current_tile_id);
						}
						return to_return;
					}
					else //if no path was found make the goal tile impassable
					{
						ecs::ECSUser::getComponentFromKnownEntity<components::TileComponent>(goalID)->impassable = true;
						to_return.push_back(startID);
						return to_return;
					}
				}
				else
				{
					to_return.push_back(startID);
					return to_return;
				}
			}
			int2 GetClosestTile(TransformComponent& transform)
			{
				//Initialize components and variables that we will need.
				int2 return_value;
				GridProp* p_gp = GridProp::GetInstance();
				unsigned int tile_index = 0;
				ecs::BaseComponent* p_base_component;
				ecs::components::TransformComponent* p_curr_tile_transform;
				ecs::components::TileComponent* p_curr_tile;
				ecs::Entity* p_curr_entity;
				float dist = 1000.0f;
				float temp_dist = 0.0f;
				//Loop through every tile in the grid.
				for (int x = 0; x < p_gp->GetSize().x; x++)
				{
					for (int y = 0; y < p_gp->GetSize().y; y++)
					{
						//Check if the tile is a tile we can walk on.
						if (p_gp->mGrid[y][x].isPassable)
						{
							p_curr_tile_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(p_gp->mGrid[y][x].Id);
							temp_dist = PhysicsHelpers::CalculateDistance(transform.position, p_curr_tile_transform->position);
							//If the tile is closer than the previously closest tile we've found we store the new info.
							if (temp_dist < dist)
							{
								dist = temp_dist;
								return_value.x = x;
								return_value.y = y;
							}
						}
					}
				}
				//Return the index position of the closest tile in mGrid in the GridProp class.
				return return_value;
			}
		private:
			unsigned int FindClosestEnemy(ecs::Entity* current_unit)
			{
				//Initialize components and variables that we will need.
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
				unsigned int enemy_id = 0;

				//Fetch the army components of all the players
				ittr = ecs::ECSUser::getComponentsOfType(ecs::components::ArmyComponent::typeID);
				while (p_base_component = ittr.next())
				{
					army_comp = static_cast<ecs::components::ArmyComponent*>(p_base_component);
					//Loop through every unit of the current army.
					for (int i = 0; i < army_comp->unitIDs.size(); i++)
					{
						other_unit = ecs::ECSUser::getEntity(army_comp->unitIDs[i]);
						//Check so that the unit still exists
						if (other_unit != nullptr)
						{
							other_unit_comp = static_cast<ecs::components::UnitComponent*>(ecs::ECSUser::getComponentFromKnownEntity(ecs::components::UnitComponent::typeID, other_unit->getID()));
							//Check so that the enemy unit is not part of the current units army.
							if (other_unit_comp->playerID != curr_unit_comp->playerID)
							{
								other_unit_transform = static_cast<ecs::components::TransformComponent*>(ecs::ECSUser::getComponentFromKnownEntity(ecs::components::TransformComponent::typeID, other_unit->getID()));
								temp_dist = PhysicsHelpers::CalculateDistance(curr_unit_transform->position, other_unit_transform->position);
								//If the distance is smaller then the previously nearest enemy we store the info of the new one.
								if (temp_dist < dist)
								{
									dist = temp_dist;
									enemy_id = other_unit->getID();
								}
							}
						}
					}
				}
				//Return the nearest enemy units entity id.
				return enemy_id;
			}

			unsigned int FindClosestFriend(ecs::Entity* current_unit)
			{
				//Initialize components and variables that we will need.
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
				unsigned int friend_id = 0;

				//Fetch the army components of all the players
				ittr = ecs::ECSUser::getComponentsOfType(ecs::components::ArmyComponent::typeID);
				while (p_base_component = ittr.next())
				{
					army_comp = static_cast<ecs::components::ArmyComponent*>(p_base_component);
					//Loop through every unit of the current army.
					for (int i = 0; i < army_comp->unitIDs.size(); i++)
					{
						other_unit = ecs::ECSUser::getEntity(army_comp->unitIDs[i]);
						//Check so that the unit still exists
						if (other_unit != nullptr)
						{
							other_unit_comp = static_cast<ecs::components::UnitComponent*>(ecs::ECSUser::getComponentFromKnownEntity(ecs::components::UnitComponent::typeID, other_unit->getID()));
							//Check so that the unit is part of the current units army.
							if (other_unit_comp->playerID == curr_unit_comp->playerID)
							{
								if (other_unit->getID() != current_unit->getID())
								{
									other_unit_transform = static_cast<ecs::components::TransformComponent*>(ecs::ECSUser::getComponentFromKnownEntity(ecs::components::TransformComponent::typeID, other_unit->getID()));
									temp_dist = PhysicsHelpers::CalculateDistance(curr_unit_transform->position, other_unit_transform->position);
									//If the distance is smaller then the previously nearest friend we store the info of the new one.
									if (temp_dist < dist)
									{
										dist = temp_dist;
										friend_id = other_unit->getID();
									}
								}
							}
							else
							{
								break;
							}
						}
					}
				}
				//Return the nearest enemy units entity id.
				return friend_id;
			}

			unsigned int FindSafeTile(ecs::Entity* current_unit)
			{
				//Fetch and initialize components and variables that we will need.
				ecs::components::UnitComponent* current_unit_comp = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::UnitComponent>(current_unit->getID());
				std::vector<ArmyComponent*> armies;
				ecs::components::UnitComponent* other_unit_comp;
				ecs::BaseComponent* p_base_comp;
				ecs::ComponentIterator it = ecs::ECSUser::getComponentsOfType(ecs::components::ArmyComponent::typeID);
				ecs::components::TransformComponent* current_tile_transform;
				ecs::components::TransformComponent* other_unit_transform;
				GridProp* p_gp = GridProp::GetInstance();
				float best_safe_value = 0;
				float safe_value = 0;
				unsigned int other_unit_id;
				unsigned int goal_id;
				//Fetch the army components of all the players.
				while (p_base_comp = it.next())
				{
					armies.push_back(static_cast<ecs::components::ArmyComponent*>(p_base_comp));
				}
				//For each tile we calculate the distance to every enemy unit and ads it together to find out which tile on the map that is the furthest from every enemy unit.
				for (int y = 0; y < MAX_ARENA_ROWS; y++)
				{
					for (int x = 0; x < MAX_ARENA_COLUMNS; x++)
					{
						if (p_gp->mGrid[y][x].isPassable)
						{
							for (int a = 0; a < armies.size(); a++)
							{
								for (int u = 0; u < armies[a]->unitIDs.size(); u++)
								{
									other_unit_id = armies[a]->unitIDs[u];
									if (ecs::ECSUser::getEntity(other_unit_id) != NULL)
									{
										other_unit_comp = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::UnitComponent>(other_unit_id);
										if (current_unit_comp->playerID != other_unit_comp->playerID)
										{
											other_unit_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(other_unit_id);
											current_tile_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(p_gp->mGrid[y][x].Id);
											safe_value += abs(PhysicsHelpers::CalculateDistance(current_tile_transform->position, other_unit_transform->position));
										}
									}
								}
							}
							//If the current tile has a better safe_value we store it.
							if (safe_value > best_safe_value)
							{
								best_safe_value = safe_value;
								goal_id = p_gp->mGrid[y][x].Id;
							}
						}
						safe_value = 0;
					}
				}
				//Return the entity id of the safest tile.
				return goal_id;
			}

			unsigned int FindClosestLootTile(ecs::Entity* current_unit)
			{
				//Initialize components and variables that we will need.
				//ecs::Entity* loot_tile;
				ecs::components::TransformComponent* unit_transform = static_cast<ecs::components::TransformComponent*>(ecs::ECSUser::getComponentFromKnownEntity(ecs::components::TransformComponent::typeID, current_unit->getID()));
				ecs::components::TransformComponent* loot_transform;
				ecs::components::TileComponent* loot_tile;
				float dist = 1000.0f;
				float temp_dist = 0.0f;
				unsigned int loot_id = 0;
				GridProp* p_gp = GridProp::GetInstance();

				for (int i = 0; i < p_gp->mLootTiles.size(); i++)
				{
					loot_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(p_gp->mLootTiles[i]);
					loot_tile = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TileComponent>(p_gp->mLootTiles[i]);
					temp_dist = PhysicsHelpers::CalculateDistance(unit_transform->position, loot_transform->position);
					if (temp_dist < dist && !loot_tile->impassable) //update if new closest has been found and it is not impassable
					{
						dist = temp_dist;
						loot_id = p_gp->mLootTiles[i];
					}
				}

				//Return the nearest enemy units entity id.
				return loot_id;
			}
		};

		/*
			A system that updates idle units. Will attack enemy units if they come to close.
		*/
		class IdleStateSystem : public ECSSystem<IdleStateSystem>
		{
		public:
			IdleStateSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::IdleStateComponent::typeID);
				typeFilter.addRequirement(components::UnitComponent::typeID);
				typeFilter.addRequirement(components::EquipmentComponent::typeID);
				typeFilter.addRequirement(components::TransformComponent::typeID);
			}
			virtual ~IdleStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				//Initialize variables.
				bool enemy_in_range = false;
				float distance = 1000.0f;
				ecs::components::AttackStateComponent atk_state;
				//Fetch data of current entity.
				ecs::components::UnitComponent* current_unit = entity.getComponent<ecs::components::UnitComponent>();
				ecs::components::TransformComponent* current_unit_transform = entity.getComponent<ecs::components::TransformComponent>();
				ecs::components::EquipmentComponent* equipment_comp = entity.getComponent<ecs::components::EquipmentComponent>();
				//Initialize data for other unit.
				ecs::components::UnitComponent* other_unit;
				ecs::components::TransformComponent* other_unit_transform;
				//Get an iterator for every unit alive.
				ecs::ComponentIterator it = ecs::ECSUser::getComponentsOfType<ecs::components::UnitComponent>();
				while (other_unit = static_cast<ecs::components::UnitComponent*>(it.next()))
				{
					//Check so that the unit we are looking at isn't a friendly unit
					if (other_unit->playerID != current_unit->playerID)
					{
						other_unit_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(other_unit->getEntityID());
						distance = PhysicsHelpers::CalculateDistance(current_unit_transform->position, other_unit_transform->position);
						//Check if the enemy is within attack range.
						if (distance < equipment_comp->mAttackRange)
						{
							//If the enemy is within attack range switch to a AttackStateComponent
							atk_state.enemyEntityId = other_unit->getEntityID();
							atk_state.activeCommand = STATE::IDLE;
							ecs::ECSUser::removeComponent(current_unit->getEntityID(), ecs::components::IdleStateComponent::typeID);
							ecs::ECSUser::createComponent(current_unit->getEntityID(), atk_state);
							enemy_in_range = true;
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
				typeFilter.addRequirement(components::EquipmentComponent::typeID);
			}
			virtual ~MoveStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				//Check if it's time to switch state or if we are supposed to stay in the current one.
 				STATE newState = CheckIfGoalIsMet(entity, delta);
				//Switch state if a new state was determined in the CheckIfGoalIsMet function.
				if (newState != STATE::NONE)
				{
 					this->SwitchState(entity, newState);
				}
				else
				{
					//Fetch the move and transform component of the entity
					ecs::components::TransformComponent* transform = entity.getComponent<ecs::components::TransformComponent>();
					ecs::components::DynamicMovementComponent* dyn_move = entity.getComponent<ecs::components::DynamicMovementComponent>();
					ecs::components::MoveStateComponent* move_comp = entity.getComponent<ecs::components::MoveStateComponent>();
					//Move the unit along its path.
					if (move_comp->path.size() > 0)
					{
						ecs::components::TransformComponent* goal = getComponentFromKnownEntity<components::TransformComponent>(move_comp->path.back());
						if (goal != nullptr)
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
						}
					}
				}
			}
		private:
			float x;
			float z;
			float length;
			float mMinimumDist;
			//Returns the new state of the unit or STATE::NONE if it is supposed to stay in this state for the next update.
			STATE CheckIfGoalIsMet(FilteredEntity& entity, float delta)
			{
				STATE returnState;
				//Fetch the move and transform component of the entity
				ecs::components::TransformComponent* transform = entity.getComponent<ecs::components::TransformComponent>();
				ecs::components::DynamicMovementComponent* dyn_move = entity.getComponent<ecs::components::DynamicMovementComponent>();
				ecs::components::MoveStateComponent* move_comp = entity.getComponent<ecs::components::MoveStateComponent>();
				ecs::components::EquipmentComponent* equipment_comp = entity.getComponent<ecs::components::EquipmentComponent>();
				//Set the minimum distance to required to the goal for a state switch to occure.
				switch (move_comp->activeCommand)
				{
				case STATE::ATTACK:
					mMinimumDist = TILE_RADIUS * 2;
					returnState = STATE::ATTACK;
					break;
				case STATE::LOOT:
					mMinimumDist = TILE_RADIUS * 2;
					returnState = STATE::LOOT;
					break;
				default:
					mMinimumDist = TILE_RADIUS / 2.0f;
					returnState = STATE::IDLE;
					break;
				}
				//Check if we are close enough to our goal and return if we are. If not we set the returnState to NONE.
				ecs::components::TransformComponent* goal_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(move_comp->goalID);
				if (goal_transform)
				{
					float distance = PhysicsHelpers::CalculateDistance(transform->position, goal_transform->position);
					if (distance <= mMinimumDist)
					{
						return returnState;
					}
					else
					{
						returnState = STATE::NONE;
					}
				}
				//Check if enough time has passed for us to calculate a new path to the goal.
				move_comp->time += delta;
				if (move_comp->time > 1.0f) //Should discuss how long we should wait and if it should be frames or time?
				{
					returnState = STATE::PATHFINDING;
				}
				//Return the new state.
				return returnState;
			}
			//Switch to the next units next state
			void SwitchState(FilteredEntity& entity, STATE newState)
			{
				bool state_switched = false;
				ecs::components::MoveStateComponent* move_comp = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::MoveStateComponent>(entity.entity->getID());
				//Switch to the new state
				if (newState == STATE::PATHFINDING)
				{
					ecs::components::PathfindingStateComponent path_state;
					path_state.activeCommand = move_comp->activeCommand;
					ecs::ECSUser::createComponent(entity.entity->getID(), path_state);
					state_switched = true;
				}
				else if (newState == STATE::IDLE)
				{
					ecs::components::IdleStateComponent idle_state;
					ecs::ECSUser::createComponent(entity.entity->getID(), idle_state);
					state_switched = true;
				}
				else if (newState == STATE::ATTACK)
				{
					ecs::components::AttackStateComponent atk_state;
					atk_state.activeCommand = move_comp->activeCommand;
					atk_state.enemyEntityId = move_comp->goalID;
					ecs::ECSUser::createComponent(entity.entity->getID(), atk_state);
					state_switched = true;
				}
				else if (newState == STATE::LOOT)
				{
					ecs::components::LootStateComponent loot_state;
					loot_state.goalID = move_comp->goalID;
					ecs::ECSUser::createComponent(entity.entity->getID(), loot_state);
					state_switched = true;
				}
				//Remove the old state if a valid new state was given.
				if (state_switched)
				{
					ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::MoveStateComponent::typeID);
				}
			}
		};

		/*
			A system that makes a unit flee from enemy units.
		*/
		class FleeStateSystem : public ECSSystem<FleeStateSystem>
		{
		public:
			FleeStateSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::FleeStateComponent::typeID);
			}
			virtual ~FleeStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				    /*************************************************/
				   /*************************************************/
				  /*   FILL OUT WITH FLEE LOGIC IN ANOTHER TASK    */
				 /*************************************************/
				/*************************************************/
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
				typeFilter.addRequirement(components::TransformComponent::typeID);
				typeFilter.addRequirement(components::DynamicMovementComponent::typeID);
			}
			virtual ~LootStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				ecs::components::TransformComponent* unit_transform = entity.getComponent<ecs::components::TransformComponent>();
				ecs::components::LootStateComponent* unit_loot_comp = entity.getComponent<ecs::components::LootStateComponent>();
				ecs::components::DynamicMovementComponent* dynamic_move_comp = entity.getComponent<ecs::components::DynamicMovementComponent>();
				
				ecs::components::TransformComponent* loot_tile_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(unit_loot_comp->goalID);
				
				float dist = PhysicsHelpers::CalculateDistance(unit_transform->position, loot_tile_transform->position);
				XMFLOAT3 direction;
				float length;

				if (dist > 0.1f)
				{
					//Calculate the direction of the enemy and normalize the vector.
					direction.x = loot_tile_transform->position.x - unit_transform->position.x;
					direction.y = loot_tile_transform->position.y - unit_transform->position.y;
					direction.z = loot_tile_transform->position.z - unit_transform->position.z;
					length = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
					direction.x /= length;
					direction.y /= length;
					direction.z /= length;
					//Set the direction.
					dynamic_move_comp->mForward = direction;
					//Create an event to move the unit towards the enemy.
					MovementInputEvent move_event;
					move_event.mInput = FORWARD;
					move_event.mEntityID = entity.entity->getID();
					createEvent(move_event);
				}
				else
				{
					ecs::components::PathfindingStateComponent path_comp;
					path_comp.activeCommand == LOOT;
					ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::LootStateComponent::typeID);
					ecs::ECSUser::createComponent(entity.entity->getID(), path_comp);
				}
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
				typeFilter.addRequirement(components::EquipmentComponent::typeID);
				typeFilter.addRequirement(components::DynamicMovementComponent::typeID);
				
			}
			virtual ~AttackStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				//Fetch data of our current unit.
				ecs::components::UnitComponent* current_unit = entity.getComponent<ecs::components::UnitComponent>();
				ecs::components::TransformComponent* current_unit_transform = entity.getComponent<ecs::components::TransformComponent>();
				ecs::components::AttackStateComponent* atk_state = entity.getComponent<ecs::components::AttackStateComponent>();
				ecs::components::EquipmentComponent* equipment_comp = entity.getComponent<ecs::components::EquipmentComponent>();
				ecs::components::DynamicMovementComponent* dynamic_move_comp = entity.getComponent<ecs::components::DynamicMovementComponent>();

				//Enemy unit data
				ecs::Entity* enemy_entity = ecs::ECSUser::getEntity(atk_state->enemyEntityId);
				ecs::components::UnitComponent* enemy_unit;
				ecs::components::TransformComponent* enemy_unit_transform;

				//Distance to the enemy unit.
				float distance = 1000.0f;
				float length;
				DirectX::XMFLOAT3 direction;
				//Check if the enemy unit still exists
				if (enemy_entity)
				{
					//Fetch the enemy units data
					enemy_unit = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::UnitComponent>(atk_state->enemyEntityId);
					enemy_unit_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(enemy_entity->getID());
					//Calculate distance to the enemy unit
					distance = PhysicsHelpers::CalculateDistance(current_unit_transform->position, enemy_unit_transform->position);
					//If the enemy is within attack range make an attack
					if (distance > TILE_RADIUS / 2.0f && distance < TILE_RADIUS * 3)
					{
						//Calculate the direction of the enemy and normalize the vector.
						direction.x = enemy_unit_transform->position.x - current_unit_transform->position.x;
						direction.y = enemy_unit_transform->position.y - current_unit_transform->position.y;
						direction.z = enemy_unit_transform->position.z - current_unit_transform->position.z;
						length = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
						direction.x /= length;
						direction.y /= length;
						direction.z /= length;
						//Set the direction.
						dynamic_move_comp->mForward = direction;
						//Create an event to move the unit towards the enemy.
						MovementInputEvent move_event;
						move_event.mInput = FORWARD;
						move_event.mEntityID = entity.entity->getID();
						createEvent(move_event);	
					}
					else if (distance > TILE_RADIUS * 3) // if the enemy is to far away change state.
					{
						SwitchState(entity, atk_state->activeCommand);
					}
				}
				else //if the enemy doesn't exist anymore, change state.
				{
					SwitchState(entity, atk_state->activeCommand);
				}
			}
		private:
			//Switch to the next units next state
			void SwitchState(FilteredEntity& entity, STATE newState)
			{
				ecs::components::AttackStateComponent* atk_comp = entity.getComponent<ecs::components::AttackStateComponent>();
				switch (newState)
				{
					case ATTACK: //If the cucrent command is Attack we want to find a new target to attack.
					{
						ecs::components::PathfindingStateComponent path_state;
						path_state.activeCommand = atk_comp->activeCommand;
						ecs::ECSUser::createComponent(entity.entity->getID(), path_state);
						break;
					}
					default: //If there is no current command we want the unit to go idle.
					{
						ecs::components::IdleStateComponent idle_state;
						ecs::ECSUser::createComponent(entity.entity->getID(), idle_state);
						break;
					}
				}
				ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::AttackStateComponent::typeID);
			};
		
		};

		/*
			A system that removes units with the DeadComponent at the end of each frame. Also removes the weapon of that unit if it is a FIST weapon
			else it sets the weapons owner to 0 so that another unit can pick it up.
		*/
		class RemoveDeadUnitsSystem : public ECSSystem<RemoveDeadUnitsSystem>
		{
		public:
			RemoveDeadUnitsSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::DeadComponent::typeID);
			}
			virtual ~RemoveDeadUnitsSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{	
				// saved fo future use
				//std::cout << "Unit killed: " << entity.entity->getID() << std::endl;
				UnitComponent* p_unit = getComponentFromKnownEntity<UnitComponent>(entity.entity->getID());
				ComponentIterator itt = getComponentsOfType<ArmyComponent>();
				ArmyComponent* p_army;
				while (p_army = (ArmyComponent*)itt.next())
				{
					//Check if it is the right player.
					if (p_army->playerID == p_unit->playerID)
					{
						//Loop through the players army and remove the id of the dead unit.
						for (int i = 0; i < p_army->unitIDs.size(); i++)
						{
							if (p_army->unitIDs[i] == entity.entity->getID())
							{
								p_army->unitIDs.erase(p_army->unitIDs.begin() + i);
							}
						}
					}
				}
				//Fetch the units weapon data.
				ecs::components::EquipmentComponent* equipment_comp = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::EquipmentComponent>(entity.entity->getID());
				ecs::Entity* weapon_entity = ecs::ECSUser::getEntity(equipment_comp->mEquippedWeapon);
				ecs::components::WeaponComponent* weapon_comp = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::WeaponComponent>(equipment_comp->mEquippedWeapon);
				//Remove the weapon entity if the weapon is a FIST else set the owner of the weapon to 0 so that another unit can pick it up.
				if (weapon_comp->mType == GAME_OBJECT_TYPE_FIST)
				{
					ecs::ECSUser::removeEntity(weapon_entity->getID());
				}
				else
				{
					ecs::ECSUser::removeEntity(weapon_entity->getID());
					//weapon_comp->mOwnerEntity = 0;
				}
				//Remove the dead unit
				ecs::ECSUser::removeEntity(entity.entity->getID());
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
				if (event.getTypeID() == ecs::events::ChangeUserStateEvent::typeID)
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

					// Chagne the command on the UI
					ecs::BaseComponent* p_base_comp = getComponentFromKnownEntity<ecs::components::UITextComponent>(p_army->getEntityID());

					ecs::components::UITextComponent* p_text_comp = static_cast<ecs::components::UITextComponent*>(p_base_comp);

					switch (state)
					{
					case STATE::IDLE:
						p_text_comp->mStrText = "IDLE";
						break;
					case STATE::LOOT:
						p_text_comp->mStrText = "LOOT";
						break;
					case STATE::ATTACK:
						p_text_comp->mStrText = "ATTACK";
						break;
					case STATE::FLEE:
						p_text_comp->mStrText = "FLEE";
						break;
					default:
						p_text_comp->mStrText = "no case for state";
						break;
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
						ecs::ECSUser::removeComponent(entity_id, ecs::components::FleeStateComponent::typeID);

						// Fetch the skeleton ID to start animations based on state
						ID skeleton_id = unit->getComponentID<ecs::components::SkeletonComponent>();

						//Give the unit the new state component.
						switch (state)
						{
							case STATE::IDLE:
							{
								ecs::components::IdleStateComponent idle;
								ecs::ECSUser::createComponent(entity_id, idle);
								break;
							}
							default:
							{
								//Defaults to a path since every other command relies on moving to a destination
								ecs::components::PathfindingStateComponent path;
								path.activeCommand = state;
								ecs::ECSUser::createComponent(entity_id, path);
								break;
							}
						}
						/*Used for debugging*/
						//std::cout << "Changing state of player: " << player << " which has the entityID: " << p_army->getEntityID() << std::endl;
					}
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