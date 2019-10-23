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
					goal_id = g_prop->mGrid[goal_tile_index.y][goal_tile_index.x].Id;
				}
				else if (path_comp->goalState == STATE::LOOT)
				{
					goal_id = 117;
				}
				path = GetPath(start_tile_id, goal_id);

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
				std::vector<unsigned int> to_return;
				std::unordered_map<unsigned int, bool> have_visited;
				ecs::ComponentIterator comp_itt;
				BaseComponent* p_base_comp;
				components::TileComponent* current_tile = nullptr;
				components::TransformComponent* current_neighbour_transfrom = nullptr;
				components::TransformComponent* goal_tile_transfrom = nullptr;
				comp_itt = ecs::ECSUser::getComponentsOfType<components::TileComponent>();
				while (p_base_comp = comp_itt.next())
				{
					current_tile = static_cast<components::TileComponent*>(p_base_comp);
					have_visited[current_tile->getEntityID()] = false;
				}
				unsigned int next_tile_id = 0;
				unsigned int current_tile_id = startID;
				unsigned int last_tile_id = 0;
				float niceTry = 999.f;
				//float lastNice = 500.f;
				float dist = 0.f;
				current_tile = ecs::ECSUser::getComponentFromKnownEntity<components::TileComponent>(startID);
				current_neighbour_transfrom = ecs::ECSUser::getComponentFromKnownEntity<components::TransformComponent>(startID);
				goal_tile_transfrom = ecs::ECSUser::getComponentFromKnownEntity<components::TransformComponent>(goalID);
				while (current_tile_id != goalID /*lastNice != niceTry*/)
				{
					if(current_tile_id != last_tile_id)
					{
						have_visited[current_tile_id] = true;
						niceTry = 999.f;
						for (int i = 0; i < 6; i++)
						{	//check if neighbour is not 0 or have been visited before
							if (current_tile->neighboursIDArray[i] != 0 &&  !have_visited[current_tile->neighboursIDArray[i]])
							{	//check for the lowest niceness then that is the next tile
								current_neighbour_transfrom = ecs::ECSUser::getComponentFromKnownEntity<components::TransformComponent>(current_tile->neighboursIDArray[i]);
								dist = GridFunctions::GetDistance(current_neighbour_transfrom->position.x, current_neighbour_transfrom->position.z,
									goal_tile_transfrom->position.x, goal_tile_transfrom->position.z);
								if (ecs::ECSUser::getComponentFromKnownEntity<components::TileComponent>(current_tile->neighboursIDArray[i])->niceness + dist < niceTry)
								{
									niceTry = ecs::ECSUser::getComponentFromKnownEntity<components::TileComponent>(current_tile->neighboursIDArray[i])->niceness + dist;
									next_tile_id = current_tile->neighboursIDArray[i];
								}
							}
						}
						if (current_tile_id != goalID)
						{
							current_tile = ecs::ECSUser::getComponentFromKnownEntity<components::TileComponent>(next_tile_id);
							to_return.push_back(next_tile_id);
							last_tile_id = current_tile_id;
							current_tile_id = next_tile_id;
						}
					}
					else
					{
						for (int i = 0; i < 6; i++)
						{
							if (current_tile->neighboursIDArray[i] != 0)
							{
								//check for the lowest niceness then that is the next tile
								current_neighbour_transfrom = ecs::ECSUser::getComponentFromKnownEntity<components::TransformComponent>(current_tile->neighboursIDArray[i]);
								dist = GridFunctions::GetDistance(current_neighbour_transfrom->position.x, current_neighbour_transfrom->position.z,
									goal_tile_transfrom->position.x, goal_tile_transfrom->position.z);
								if (dist < niceTry)
								{
									niceTry = dist;
									next_tile_id = current_tile->neighboursIDArray[i];
								}
							}
							current_tile = ecs::ECSUser::getComponentFromKnownEntity<components::TileComponent>(next_tile_id);
							to_return.push_back(next_tile_id);
							last_tile_id = current_tile_id;
							current_tile_id = next_tile_id;
						}
					}
				}
				return to_return;
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
							atk_state.goalState = STATE::IDLE;
							ecs::ECSUser::removeComponent(current_unit->getEntityID(), ecs::components::IdleStateComponent::typeID);
							ecs::ECSUser::createComponent(current_unit->getEntityID(), atk_state);
							enemy_in_range = true;
							break;
						}
					}
				}
				//If no enemy is in range continue with idle animation.
				if (!enemy_in_range) 
				{
					/**********************************************************/
					/**********************************************************/
					/* FILL OUT WITH LOGIC FOR IDLE ANIMATION IN ANOTHER TASK */
					/**********************************************************/
					/**********************************************************/
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
				//Fetch the move and transform component of the entity
				ecs::components::TransformComponent* transform = entity.getComponent<ecs::components::TransformComponent>();
				ecs::components::DynamicMovementComponent* dyn_move = entity.getComponent<ecs::components::DynamicMovementComponent>();
				ecs::components::MoveStateComponent* move_comp = entity.getComponent<ecs::components::MoveStateComponent>();
				//Move the unit along its path.
				if (move_comp->path.size() > 0)
				{
					ecs::components::TransformComponent* goal = getComponentFromKnownEntity<components::TransformComponent>(move_comp->path.front());
					if (abs(goal->position.x - transform->position.x) < 0.3f && abs(goal->position.z - transform->position.z) < 0.3f)
					{
						move_comp->path.erase(move_comp->path.begin());
					}
					if (move_comp->path.size() > 0)
					{
						goal = getComponentFromKnownEntity<components::TransformComponent>(move_comp->path.front());
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
					//Check if it's time to switch state or if we are supposed to stay in the current one.
					STATE newState = CheckIfGoalIsMet(entity, delta);
					//Switch state if a new state was determined in the CheckIfGoalIsMet function.
					if (newState != STATE::NONE)
					{
						SwitchState(entity, newState);
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
				switch (move_comp->goalState)
				{
				case STATE::ATTACK:
					mMinimumDist = 1000.0f;//equipment_comp->mAttackRange;
					returnState = STATE::ATTACK;
					break;
				case STATE::LOOT:
					mMinimumDist = TILE_RADIUS / 2.0f;
					returnState = STATE::LOOT;
					break;
				default:
					mMinimumDist = TILE_RADIUS / 2.0f;
					returnState = STATE::IDLE;
					break;
				}
				//Check if we are close enough to our goal and return if we are. If not we set the returnState to NONE.
				float distance = PhysicsHelpers::CalculateDistance(transform->position, move_comp->goalPos);
				if (distance <= mMinimumDist)
				{
					return returnState;
				}
				else
				{
					returnState = STATE::NONE;
				}
				//Check if enough time has passed for us to calculate a new path to the goal.
				move_comp->time += delta;
				if (move_comp->time > 5.0f) //Should discuss how long we should wait and if it should be frames or time?
				{
					returnState = STATE::PATHFINDING;
				}
				//Return the new state.
				return returnState;
			};
			//Switch to the next units next state
			void SwitchState(FilteredEntity& entity, STATE newState)
			{
				ecs::components::MoveStateComponent* move_comp = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::MoveStateComponent>(entity.entity->getID());
				switch (newState)
				{
					case IDLE:
					{
						ecs::components::IdleStateComponent idle_state;
						ecs::ECSUser::createComponent(entity.entity->getID(), idle_state);
						break;
					}
					case ATTACK:
					{
						ecs::components::AttackStateComponent atk_state;
						atk_state.goalState = move_comp->goalState;
						atk_state.enemyEntityId = move_comp->goalID;
						ecs::ECSUser::createComponent(entity.entity->getID(), atk_state);
						break;
					}
					case LOOT:
					{
						ecs::components::LootStateComponent loot_state;
						ecs::ECSUser::createComponent(entity.entity->getID(), loot_state);
						break;
					}
					case PATHFINDING:
					{
						ecs::components::PathfindingStateComponent path_state;
						path_state.goalState = move_comp->goalState;
						break;
					}
					default:
						break;
				}
				ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::MoveStateComponent::typeID);
			};
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
				ecs::components::TransformComponent* current_unit_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(current_unit->getEntityID());
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
					//If the enemy is within attack range make an attack else switch to pathfinding state
					if (distance < 1000.0f/*equipment_comp->mAttackRange*/)
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
					else
					{
						SwitchState(entity, atk_state->goalState);
					}
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
						path_state.goalState = atk_comp->goalState;
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