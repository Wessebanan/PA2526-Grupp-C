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
				int2 start_tile = 0;
				std::vector<unsigned int> path;
				GridProp* g_prop = GridProp::GetInstance();
				components::TransformComponent* p_transfrom = entity.getComponent<TransformComponent>();
				start_tile = GridFunctions::GetTileFromWorldPos(p_transfrom->position.x, p_transfrom->position.z);
				unsigned int start_tile_id = g_prop->mGrid[start_tile.y][start_tile.x].Id;
				path = GetPath(start_tile_id,113);
				components::MoveStateComponent move_comp;
				move_comp.path = path;
				
				ecs::ECSUser::createComponent(entity.entity->getID(), move_comp);
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
				float lastNice = 500.f;
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
						{	//check if neighbour is not 0 or was the last visited tile
							if (current_tile->neighboursIDArray[i] != 0 && !have_visited[current_tile->neighboursIDArray[i]])
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
				}
				return to_return;
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
			}
			virtual ~IdleStateSystem() {}

			//Update function that prints the center position of every tile in the order they 
			//were created.
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				/* FILL OUT WITH LOGIC IN ANOTHER TASK */
			}
		};

		/*
			A system that moving units.
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
				ecs::components::TransformComponent* transform = entity.getComponent<ecs::components::TransformComponent>();
				ecs::components::DynamicMovementComponent* dyn_move = entity.getComponent<ecs::components::DynamicMovementComponent>();
				ecs::components::MoveStateComponent* move_comp = entity.getComponent<ecs::components::MoveStateComponent>();
				if(move_comp->path.size() > 0)
				{
					ecs::components::TransformComponent* goal = getComponentFromKnownEntity<components::TransformComponent>(move_comp->path.front());				
					if (abs(goal->position.x - transform->position.x) < 0.005f && abs(goal->position.z - transform->position.z) < 0.005f)
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
						createEvent(kek);
					}
				}
				
			}
		private:
			float x;
			float z;
			float length;
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
				int state = static_cast<ecs::events::ChangeUserStateEvent*>(&event)->newState;
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
					if (unit->hasComponentOfType(ecs::components::MoveStateComponent::typeID))
					{
						ecs::ECSUser::removeComponent(entity_id, ecs::components::MoveStateComponent::typeID);
					}
					else if (unit->hasComponentOfType(ecs::components::IdleStateComponent::typeID))
					{
						ecs::ECSUser::removeComponent(entity_id, ecs::components::IdleStateComponent::typeID);
					}
					else if (unit->hasComponentOfType(ecs::components::PathfindingStateComponent::typeID))
					{
						ecs::ECSUser::removeComponent(entity_id, ecs::components::PathfindingStateComponent::typeID);
					}

					//Create one instance of each possible component to use in the switch case (Ugly will have to find a better way later).
					ecs::components::MoveStateComponent move;
					ecs::components::PathfindingStateComponent path;
					ecs::components::IdleStateComponent idle;
					//Give the unit the new state component.
					switch (state)
					{
					case STATE::MOVE:
						ecs::ECSUser::createComponent(entity_id, path);
						break;
					case STATE::IDLE:
						ecs::ECSUser::createComponent(entity_id, idle);
						break;
					case STATE::ATTACK:
						break;
					default:
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
