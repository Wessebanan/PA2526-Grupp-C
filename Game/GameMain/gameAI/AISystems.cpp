#include "AISystems.h"

using namespace ecs::components;
using namespace ecs::events;

/************************************************/
/*************  TILEPRINTSYSTEM START  *********/
/**********************************************/
ecs::systems::TilePrintSystem::TilePrintSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(TileComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
}

ecs::systems::TilePrintSystem::~TilePrintSystem()
{

}

void ecs::systems::TilePrintSystem::updateEntity(FilteredEntity& entity, float delta)
{
	TransformComponent* pTc = entity.getComponent<TransformComponent>();
	std::cout << "Entity: " << pTc->getEntityID() << " Position: " << pTc->position.x << " , " << pTc->position.y << " , " << pTc->position.z << std::endl;
}

/************************************************/
/*************  TILEPRINTSYSTEM END  ***********/
/**********************************************/

/************************************************/
/******  PATHFINDINGSTATESYSTEM START  *********/
/**********************************************/

ecs::systems::PathfindingStateSystem::PathfindingStateSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(PathfindingStateComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
}

ecs::systems::PathfindingStateSystem::~PathfindingStateSystem()
{

}

void ecs::systems::PathfindingStateSystem::updateEntity(FilteredEntity& entity, float delta)
{
	PathfindingStateComponent* path_comp = entity.getComponent<PathfindingStateComponent>();
	int2 start_tile = 0;
	std::vector<unsigned int> path;
	GridProp* g_prop = GridProp::GetInstance();
	TransformComponent* p_transfrom = entity.getComponent<TransformComponent>();
	TransformComponent* goal_transform = nullptr;
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
			goal_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(goal_enemy_id);
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
		
		EquipmentComponent* equipment_comp = ECSUser::getComponentFromKnownEntity<EquipmentComponent>(entity.entity->getID());
		WeaponComponent* weapon_comp = ECSUser::getComponentFromKnownEntity<WeaponComponent>(equipment_comp->mEquippedWeapon);
		//Check if the unit have a weapon already. If so find a friendly unit without a weapon and follow that unit.
		if (weapon_comp->mType != GAME_OBJECT_TYPE_WEAPON_FIST)
		{
			goal_friend_id = this->FindClosestFriendWithoutWeapon(entity.entity);
			if (goal_friend_id != 0)
			{
				//Find the closest tile to the nearest friend without a weapon.
				goal_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(goal_friend_id);
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
		//If this unit doesn't have a weapon or if there was no friendly unit without a weapon we find the closest loot tile and move to it.
		if (goal_friend_id == 0)
		{
			goal_id = this->FindClosestLootTile(entity.entity);
		}
		//If we found a valid goal we calculate a path to it.
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
	case STATE::RALLY:
	{
		goal_friend_id = this->FindClosestFriend(entity.entity);
		if (goal_friend_id != 0)
		{
			//Find the closest tile to the nearest friend.
			goal_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(goal_friend_id);
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
		MoveStateComponent move_comp;
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
		else if (move_comp.activeCommand == FLEE)
		{
			move_comp.goalID = goal_id;
			FleeStateComponent flee_comp;
			flee_comp.activeCommand = FLEE;
			ECSUser::createComponent(entity.entity->getID(), flee_comp);
		}
		else if (move_comp.activeCommand == RALLY)
		{
			move_comp.goalID = goal_friend_id;
		}
		else
		{
			move_comp.goalID = goal_id;
		}
		//Create the move component and hand it to the unit.
		ECSUser::createComponent(entity.entity->getID(), move_comp);
	}
	else
	{
		//If no valid goal was found we set the unit to idle.
		IdleStateComponent idle_comp;
		idle_comp.activeCommand = path_comp->activeCommand;
		ECSUser::createComponent(entity.entity->getID(), idle_comp);
	}
	//Remove the PathfindingStateComponent
	ECSUser::removeComponent(entity.entity->getID(), PathfindingStateComponent::typeID);
}

std::vector<unsigned int> ecs::systems::PathfindingStateSystem::GetPath(unsigned int startID, unsigned int goalID)
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
	TileComponent* current_tile = nullptr;
	TileComponent* current_neighbour_tile = nullptr;
	TransformComponent* current_neighbour_transfrom = nullptr;
	TransformComponent* goal_tile_transfrom = nullptr;

	float cost = 999.f;
	bool in_open_list = false;
	float dist_to_goal = 0.f;
	int pos_in_open_list = 0;
	unsigned int parent_id = 0;
	unsigned int next_tile_id = 0;
	float best_neighbour_cost = 999.f;
	unsigned int current_tile_id = startID;
	bool no_path_found = false;

	current_tile = ECSUser::getComponentFromKnownEntity<TileComponent>(startID);
	goal_tile_transfrom = ECSUser::getComponentFromKnownEntity<TransformComponent>(goalID);

	GridProp* p_gp = GridProp::GetInstance();

	//Add the start tile to the closed list
	NodeInfo start_node;
	start_node.id = startID;
	start_node.parent_id = startID;
	start_node.move_cost = 0;
	closed_list[current_tile_id] = start_node;
	if (current_tile != nullptr && goal_tile_transfrom != nullptr) // maybe remove later because they should never be sent to pathfinding of they are null
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
						current_neighbour_transfrom = ECSUser::getComponentFromKnownEntity<TransformComponent>(current_tile->neighboursIDArray[i]);
						current_neighbour_tile = ECSUser::getComponentFromKnownEntity<TileComponent>(current_tile->neighboursIDArray[i]);
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
						current_neighbour_transfrom = ECSUser::getComponentFromKnownEntity<TransformComponent>(current_tile->neighboursIDArray[i]);
						current_neighbour_tile = ECSUser::getComponentFromKnownEntity<TileComponent>(current_tile->neighboursIDArray[i]);
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
			if (open_list.size() > 0)
			{
				open_list.erase(open_list.begin() + pos_in_open_list);	//remove from openlist
			}
			current_tile = ECSUser::getComponentFromKnownEntity<TileComponent>(next_tile_id);
			current_tile_id = next_tile_id;

		}
		//when we have found the goal we start to build the path by starting with the goal tile then we add that tiles parent tile and do this until we come to the start tile
		//parent tile in this function is the tile that was used to get to the child tile, so when we find the goal we just the go backwards using the parents
		if (!no_path_found) //if path was found
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
			ECSUser::getComponentFromKnownEntity<TileComponent>(goalID)->impassable = true;
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

int2 ecs::systems::PathfindingStateSystem::GetClosestTile(TransformComponent& transform)
{
	//Initialize components and variables that we will need.
	int2 return_value;
	GridProp* p_gp = GridProp::GetInstance();
	unsigned int tile_index = 0;
	BaseComponent* p_base_component;
	TransformComponent* p_curr_tile_transform;
	TileComponent* p_curr_tile;
	Entity* p_curr_entity;
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
				p_curr_tile_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(p_gp->mGrid[y][x].Id);
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

unsigned int ecs::systems::PathfindingStateSystem::FindClosestEnemy(Entity* current_unit)
{
	//Initialize components and variables that we will need.
	ComponentIterator ittr;
	BaseComponent* p_base_component;
	ArmyComponent* army_comp;
	Entity* other_unit;
	UnitComponent* curr_unit_comp = ECSUser::getComponentFromKnownEntity<UnitComponent>(current_unit->getID());
	UnitComponent* other_unit_comp;
	TransformComponent* curr_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(current_unit->getID());
	TransformComponent* other_unit_transform;
	float dist = 1000.0f;
	float temp_dist = 0.0f;
	unsigned int enemy_id = 0;

	//Fetch the army components of all the players
	ittr = ECSUser::getComponentsOfType(ArmyComponent::typeID);
	while (p_base_component = ittr.next())
	{
		army_comp = static_cast<ArmyComponent*>(p_base_component);
		//Loop through every unit of the current army.
		for (int i = 0; i < army_comp->unitIDs.size(); i++)
		{
			other_unit = ECSUser::getEntity(army_comp->unitIDs[i]);
			//Check so that the unit still exists
			if (other_unit != nullptr)
			{
				other_unit_comp = ECSUser::getComponentFromKnownEntity<UnitComponent>(other_unit->getID());
				//Check so that the enemy unit is not part of the current units army.
				if (other_unit_comp->playerID != curr_unit_comp->playerID)
				{
					other_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(other_unit->getID());
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

unsigned int ecs::systems::PathfindingStateSystem::FindClosestFriendWithoutWeapon(Entity* current_unit)
{
	//Initialize components and variables that we will need.
	ComponentIterator ittr;
	BaseComponent* p_base_component;
	ArmyComponent* army_comp;
	Entity* other_unit;
	UnitComponent* curr_unit_comp = ECSUser::getComponentFromKnownEntity<UnitComponent>(current_unit->getID());
	UnitComponent* other_unit_comp;
	TransformComponent* curr_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(current_unit->getID());
	TransformComponent* other_unit_transform;
	Entity* other_unit_weapon;
	float dist = 1000.0f;
	float temp_dist = 0.0f;
	unsigned int friend_id = 0;

	//Fetch the army components of all the players
	ittr = ECSUser::getComponentsOfType(ArmyComponent::typeID);
	while (p_base_component = ittr.next())
	{
		army_comp = static_cast<ArmyComponent*>(p_base_component);
		//Loop through every unit of the current army.
		for (int i = 0; i < army_comp->unitIDs.size(); i++)
		{
			other_unit = ECSUser::getEntity(army_comp->unitIDs[i]);
			//Check so that the unit still exists
			if (other_unit != nullptr)
			{
				other_unit_comp = static_cast<UnitComponent*>(ECSUser::getComponentFromKnownEntity(UnitComponent::typeID, other_unit->getID()));
				//Check so that the unit is part of the current units army.
				if (other_unit_comp->playerID == curr_unit_comp->playerID)
				{
					if (other_unit->getID() != current_unit->getID())
					{
						other_unit_weapon = ECSUser::getEntity(ECSUser::getComponentFromKnownEntity<EquipmentComponent>(other_unit->getID())->mEquippedWeapon);
						WeaponComponent* other_unit_weapon_comp = ECSUser::getComponentFromKnownEntity<WeaponComponent>(other_unit_weapon->getID());
						//Check if the friendly unit is without a weapon. We only want to follow allies without weapons while they loot.
						if (other_unit_weapon_comp->mType == GAME_OBJECT_TYPE_WEAPON_FIST)
						{
							other_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(other_unit->getID());
							temp_dist = PhysicsHelpers::CalculateDistance(curr_unit_transform->position, other_unit_transform->position);
							//If the distance is smaller then the previously nearest friend we store the info of the new one.
							if (temp_dist < dist)
							{
								dist = temp_dist;
								friend_id = other_unit->getID();
							}
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

unsigned int ecs::systems::PathfindingStateSystem::FindClosestFriend(ecs::Entity* current_unit)
{
	//Initialize components and variables that we will need.
	ComponentIterator ittr;
	BaseComponent* p_base_component;
	ArmyComponent* army_comp;
	Entity* other_unit;
	UnitComponent* curr_unit_comp = ECSUser::getComponentFromKnownEntity<UnitComponent>(current_unit->getID());
	UnitComponent* other_unit_comp;
	TransformComponent* curr_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(current_unit->getID());
	TransformComponent* other_unit_transform;
	float dist = 1000.0f;
	float temp_dist = 0.0f;
	unsigned int friend_id = 0;

	//Fetch the army components of all the players
	ittr = ECSUser::getComponentsOfType(ArmyComponent::typeID);
	while (p_base_component = ittr.next())
	{
		army_comp = static_cast<ArmyComponent*>(p_base_component);
		//Loop through every unit of the current army.
		for (int i = 0; i < army_comp->unitIDs.size(); i++)
		{
			other_unit = ECSUser::getEntity(army_comp->unitIDs[i]);
			//Check so that the unit still exists
			if (other_unit != nullptr)
			{
				other_unit_comp = static_cast<UnitComponent*>(ECSUser::getComponentFromKnownEntity(UnitComponent::typeID, other_unit->getID()));
				//Check so that the unit is part of the current units army.
				if (other_unit_comp->playerID == curr_unit_comp->playerID)
				{
					if (other_unit->getID() != current_unit->getID())
					{
						other_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(other_unit->getID());
						temp_dist = PhysicsHelpers::CalculateDistance(curr_unit_transform->position, other_unit_transform->position);
						//If the distance is smaller then the previously nearest friend we store the info of the new one.
						if (temp_dist < dist && temp_dist)
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

unsigned int ecs::systems::PathfindingStateSystem::FindSafeTile(Entity* current_unit)
{
	UnitComponent* p_current_unit_comp = ECSUser::getComponentFromKnownEntity<UnitComponent>(current_unit->getID());
	TransformComponent* p_current_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(current_unit->getID());
	int2 start_tile_index = GetClosestTile(*p_current_unit_transform);
	GridProp* p_gp = GridProp::GetInstance();
	ID start_tile_id = p_gp->mGrid[start_tile_index.y][start_tile_index.x].Id;

	TileComponent* p_current_tile = ECSUser::getComponentFromKnownEntity<TileComponent>(start_tile_id);
	TileComponent* p_current_neighbour;
	int goal_tile_id = 0;
	float current_safe = 0.0f;
	float safest = 1000.0f;
	for (int i = 0; i < 6; i++)
	{
		if (p_current_tile->neighboursIDArray[i] != 0)
		{
			p_current_neighbour = ECSUser::getComponentFromKnownEntity<TileComponent>(p_current_tile->neighboursIDArray[i]);
			if (p_current_neighbour != nullptr) //Sanity Check
			{
				for (int a = 0; a < 4; a++)
				{
					if (a != p_current_unit_comp->playerID)
					{
						current_safe += p_current_neighbour->charges.armyCharges[a];
					}
				}
				//current_safe += p_current_neighbour->charges.hazardCharge;
				if (current_safe < safest)
				{
					safest = current_safe;
					goal_tile_id = p_current_tile->neighboursIDArray[i];
				}
				current_safe = 0.0f;
			}
		}
	}
	return goal_tile_id;
}

unsigned int ecs::systems::PathfindingStateSystem::FindClosestLootTile(Entity* current_unit)
{
	//Initialize components and variables that we will need.
				//ecs::Entity* loot_tile;
	TransformComponent* unit_transform = static_cast<TransformComponent*>(ECSUser::getComponentFromKnownEntity(TransformComponent::typeID, current_unit->getID()));
	TransformComponent* loot_transform;
	TileComponent* loot_tile;
	float dist = 1000.0f;
	float temp_dist = 0.0f;
	unsigned int loot_id = 0;
	GridProp* p_gp = GridProp::GetInstance();
	if (p_gp != nullptr) //Sanity check
	{
		for (int i = 0; i < p_gp->mLootTiles.size(); i++)
		{
			loot_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(p_gp->mLootTiles[i]);
			loot_tile = ECSUser::getComponentFromKnownEntity<TileComponent>(p_gp->mLootTiles[i]);
			temp_dist = PhysicsHelpers::CalculateDistance(unit_transform->position, loot_transform->position);
			if (temp_dist < dist && !loot_tile->impassable) //update if new closest has been found and it is not impassable
			{
				dist = temp_dist;
				loot_id = p_gp->mLootTiles[i];
			}
		}
	}

	//Return the nearest enemy units entity id.
	return loot_id;
}

/************************************************/
/*******  PATHFINDINGSTATESYSTEM END  **********/
/**********************************************/

/************************************************/
/*********  IDLESTATESYSTEM START  *************/
/**********************************************/

ecs::systems::IdleStateSystem::IdleStateSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(IdleStateComponent::typeID);
	typeFilter.addRequirement(UnitComponent::typeID);
	typeFilter.addRequirement(EquipmentComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
}

ecs::systems::IdleStateSystem::~IdleStateSystem()
{

}

void ecs::systems::IdleStateSystem::updateEntity(FilteredEntity& entity, float delta)
{
	//Initialize variables.
	float distance = 1000.0f;
	//Fetch data of current entity.
	UnitComponent* p_current_unit = entity.getComponent<UnitComponent>();
	TransformComponent* p_current_unit_transform = entity.getComponent<TransformComponent>();
	EquipmentComponent* p_equipment_comp = entity.getComponent<EquipmentComponent>();
	IdleStateComponent* p_idle_comp = entity.getComponent<IdleStateComponent>();
	//Initialize data for other unit.
	UnitComponent* p_other_unit;
	TransformComponent* p_other_unit_transform;
	//Get an iterator for every unit alive.
	ComponentIterator it = ECSUser::getComponentsOfType<UnitComponent>();
	while (p_other_unit = static_cast<UnitComponent*>(it.next()))
	{
		//Check so that the unit we are looking at isn't a friendly unit
		if (p_other_unit->playerID != p_current_unit->playerID)
		{
			p_other_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(p_other_unit->getEntityID());
			distance = PhysicsHelpers::CalculateDistance(p_current_unit_transform->position, p_other_unit_transform->position);
			if (p_idle_comp->activeCommand == IDLE)
			{
				//Check if the enemy is within attack range.
				if (distance < p_equipment_comp->mAttackRange * 1.5f)
				{
					//If the enemy is within attack range switch to a AttackStateComponent
					AttackStateComponent atk_state;
					atk_state.enemyEntityId = p_other_unit->getEntityID();
					atk_state.activeCommand = STATE::IDLE;
					ECSUser::removeComponent(p_current_unit->getEntityID(), IdleStateComponent::typeID);
					ECSUser::createComponent(p_current_unit->getEntityID(), atk_state);
					break;
				}
			}
			else if (p_idle_comp->activeCommand == FLEE)
			{
				if (distance < TILE_RADIUS * 15)
				{
					PathfindingStateComponent pathfinding_state;
					pathfinding_state.activeCommand = FLEE;
					FleeStateComponent flee_state;
					ECSUser::removeComponent(p_current_unit->getEntityID(), IdleStateComponent::typeID);
					ECSUser::createComponent(p_current_unit->getEntityID(), flee_state);
					ECSUser::createComponent(p_current_unit->getEntityID(), pathfinding_state);
					break;
				}
			}
		}
	}
}

/************************************************/
/**********  IDLESTATESYSTEM END  **************/
/**********************************************/

/************************************************/
/*********  MOVESTATESYSTEM START  *************/
/**********************************************/

ecs::systems::MoveStateSystem::MoveStateSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(MoveStateComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
	typeFilter.addRequirement(DynamicMovementComponent::typeID);
	typeFilter.addRequirement(EquipmentComponent::typeID);
	typeFilter.addRequirement(GroundCollisionComponent::typeID);
}

ecs::systems::MoveStateSystem::~MoveStateSystem()
{

}

void ecs::systems::MoveStateSystem::updateEntity(FilteredEntity& entity, float delta)
{
	//If unit is supposed to be idle return.
	if (entity.entity->hasComponentOfType(IdleStateComponent::typeID))
	{
		ECSUser::removeComponent(entity.entity->getID(), MoveStateComponent::typeID);
		return;
	}
	//Check if it's time to switch state or if we are supposed to stay in the current one.
	STATE newState = CheckIfGoalIsMet(entity, delta);
	//Switch state if a new state was determined in the CheckIfGoalIsMet function.
	if (newState != STATE::NONE)
	{
		this->SwitchState(entity, newState);
	}
	//Fetch the move and transform component of the entity
	TransformComponent* p_transform = entity.getComponent<TransformComponent>();
	DynamicMovementComponent* p_dyn_move = entity.getComponent<DynamicMovementComponent>();
	MoveStateComponent* p_move_comp = entity.getComponent<MoveStateComponent>();
	GroundCollisionComponent* p_ground_comp = entity.getComponent<GroundCollisionComponent>();
	EquipmentComponent* p_equipment_comp = entity.getComponent<EquipmentComponent>();
	TransformComponent* p_goal = ECSUser::getComponentFromKnownEntity<TransformComponent>(p_move_comp->goalID);
	float distance = 1000.0f;
	if (p_goal != nullptr)
	{
		distance = PhysicsHelpers::CalculateDistance(p_transform->position, p_goal->position);
	}
	if (distance > TILE_RADIUS * 2.0f)
	{
		//Move the unit along its path.
		if (p_move_comp->path.size() > 0)
		{
			p_goal = getComponentFromKnownEntity<TransformComponent>(p_move_comp->path.back());
			if (p_goal != nullptr)
			{
				ID unit_tile = this->GetClosestTileId(*p_transform);
				if (unit_tile == p_move_comp->path.back())
				{
					p_move_comp->path.pop_back();
					if (p_move_comp->path.size() > 0)
					{
						p_goal = getComponentFromKnownEntity<TransformComponent>(p_move_comp->path.back());//get next tile towards goal, "local goal"
					}
					else
					{
						p_goal = nullptr;
					}
				}
			}

		}
	}
	else if (distance < p_equipment_comp->mAttackRange * 0.8f && p_move_comp->activeCommand == ATTACK)
	{
		p_goal = nullptr;
	}

	if (p_goal != nullptr)
	{
		float curr_tile_x;
		float curr_tile_y;
		float curr_tile_z;
		float next_tile_x = 0;
		float next_tile_z = 0;
		float y_distance;
		float length;
		float length_of_vector;
		float angle;
		XMFLOAT3 jump_vector;
		if (p_move_comp->path.size() > 1)//if there are 2 or more tiles left we get the tile after the next
		{
			TransformComponent* p_next_goal = getComponentFromKnownEntity<TransformComponent>(p_move_comp->path.at(p_move_comp->path.size() - 2));
			next_tile_x = p_next_goal->position.x - p_goal->position.x;
			next_tile_z = p_next_goal->position.z - p_goal->position.z;
		}
		jump_vector.x = curr_tile_x = p_goal->position.x - p_transform->position.x;
		jump_vector.y = curr_tile_y = p_goal->position.y - p_ground_comp->mLastTileY;
		jump_vector.z = curr_tile_z = p_goal->position.z - p_transform->position.z;
		y_distance = p_goal->position.y - (p_ground_comp->mLastTileY);
		curr_tile_x = curr_tile_x + (next_tile_x * DEFAULT_USAGE_OF_TILE);//ad percentage of the direction from the tile next after "goal"
		curr_tile_z = curr_tile_z + (next_tile_z * DEFAULT_USAGE_OF_TILE);
		length = sqrt(curr_tile_x * curr_tile_x + curr_tile_z * curr_tile_z);
		p_dyn_move->mForward.x = curr_tile_x / length;
		p_dyn_move->mForward.z = curr_tile_z / length;

		if (ECSUser::getComponentFromKnownEntity<UnitComponent>(p_goal->getEntityID()))//if the target is a unit we check y-value from same origin
		{
			y_distance = p_goal->position.y - p_transform->position.y;
		}

		if (y_distance > 0.3f && p_dyn_move->mOnGround)
		{
			length = PhysicsHelpers::CalculateDistance(p_goal->position, p_transform->position);//Length from unit to goal center
			length_of_vector = XMVectorGetX(XMVector3Length(XMLoadFloat3(&p_dyn_move->mVelocity)));//Length of velocity vector
			angle = XMVectorGetX(XMVector3Dot(XMVector3Normalize
			(XMLoadFloat3(&p_dyn_move->mVelocity)), XMVector3Normalize(XMLoadFloat3(&p_dyn_move->mDirection))));//Get angle between velocity and direction vector
			//if their velocity vector is same or larger then the vector between their position and the edge of a tile
			//and they move in the same direction as they are looking
			if ((length_of_vector >= (length - DEFAULT_TILE_SIDE_LENGTH)) && angle > 0.9f)
			{
				//modify values so that they jump more upwards
				jump_vector.x /= 7.f;
				jump_vector.y *= 8.f;
				jump_vector.z /= 7.f;

				ForceImpulseEvent jump;
				XMStoreFloat3(&jump.mDirection, XMVector3Normalize(XMLoadFloat3(&jump_vector)));//normalize the jump vector so that we just get direction
				jump.mForce = ((sqrtf(2.f * y_distance * p_dyn_move->mGravity)) * p_dyn_move->mWeight) * 1.2f;
				jump.mEntityID = entity.entity->getID();
				if (length_of_vector < 0.25f)//if they are very slow and need to jump they get a boost
				{
					jump.mForce *= 1.25f;
					jump_vector.x *= 1.1f;
					jump_vector.z *= 1.1f;

				}
				createEvent(jump);
			}
		}
		MovementInputEvent move;
		move.mInput = FORWARD;
		move.mEntityID = entity.entity->getID();
		createEvent(move);//creates an event to physics to move character
	}	
}

STATE ecs::systems::MoveStateSystem::CheckIfGoalIsMet(FilteredEntity& entity, float delta)
{
	STATE returnState;
	//Fetch the move and transform component of the entity
	TransformComponent* transform = entity.getComponent<TransformComponent>();
	DynamicMovementComponent* dyn_move = entity.getComponent<DynamicMovementComponent>();
	MoveStateComponent* move_comp = entity.getComponent<MoveStateComponent>();
	EquipmentComponent* equipment_comp = entity.getComponent<EquipmentComponent>();
	//Set the minimum distance to required to the goal for a state switch to occure.
	switch (move_comp->activeCommand)
	{
	case STATE::ATTACK:
		mMinimumDist = equipment_comp->mAttackRange * 0.9f;
		returnState = STATE::ATTACK;
		break;
	case STATE::LOOT:
		mMinimumDist = TILE_RADIUS / 2.0f;
		returnState = STATE::LOOT;
		break;
	case STATE::RALLY:
		mMinimumDist = TILE_RADIUS * 1.0f;
		returnState = STATE::IDLE; 
		break;
	default:
		mMinimumDist = TILE_RADIUS / 2.0f;
		returnState = STATE::IDLE;
		break;
	}
	//Check if we are close enough to our goal and return if we are. If not we set the returnState to NONE.
	TransformComponent* goal_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(move_comp->goalID);
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

void ecs::systems::MoveStateSystem::SwitchState(FilteredEntity& entity, STATE newState)
{
	bool state_switched = false;
	MoveStateComponent* move_comp = ECSUser::getComponentFromKnownEntity<MoveStateComponent>(entity.entity->getID());
	//Switch to the new state
	if (newState == STATE::PATHFINDING)
	{
		PathfindingStateComponent path_state;
		path_state.activeCommand = move_comp->activeCommand;
		ECSUser::createComponent(entity.entity->getID(), path_state);
		state_switched = true;
	}
	else if (newState == STATE::IDLE)
	{
		if (!entity.entity->hasComponentOfType(IdleStateComponent::typeID))
		{
			IdleStateComponent idle_state;
			idle_state.activeCommand = IDLE;
			ECSUser::createComponent(entity.entity->getID(), idle_state);
		}
	}
	else if (newState == STATE::ATTACK)
	{
		if (!entity.entity->hasComponentOfType(AttackStateComponent::typeID))
		{
			AttackStateComponent atk_state;
			atk_state.activeCommand = move_comp->activeCommand;
			atk_state.enemyEntityId = move_comp->goalID;
			ECSUser::createComponent(entity.entity->getID(), atk_state);
		}
	}
	else if (newState == STATE::LOOT)
	{
		if (!entity.entity->hasComponentOfType(LootStateComponent::typeID))
		{
			LootStateComponent loot_state;
			loot_state.goalID = move_comp->goalID;
			ECSUser::createComponent(entity.entity->getID(), loot_state);
		}
	}
	//Remove the old state if a valid new state was given.
	if (state_switched)
	{
		ECSUser::removeComponent(entity.entity->getID(), ecs::components::MoveStateComponent::typeID);
	}
}

ID ecs::systems::MoveStateSystem::GetClosestTileId(TransformComponent& transform)
{
	//Initialize components and variables that we will need.
	int2 tile_index;
	GridProp* p_gp = GridProp::GetInstance();
	ecs::BaseComponent* p_base_component;
	ecs::components::TransformComponent* p_curr_tile_transform;
	ecs::components::TileComponent* p_curr_tile;
	ecs::Entity* p_curr_entity;
	float dist = 1000.0f;
	float temp_dist = 0.0f;
	ID return_id = 0;
	if (p_gp != nullptr) //Sanity Check
	{
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
						tile_index.x = x;
						tile_index.y = y;
					}
				}
			}
		}
		return_id = p_gp->mGrid[tile_index.y][tile_index.x].Id;
	}
	
	//Return the index position of the closest tile in mGrid in the GridProp class or 0 if something went wrong.
	return return_id;
}
/************************************************/
/**********  MOVESTATESYSTEM END  **************/
/**********************************************/

/************************************************/
/*********  FLEESTATESYSTEM START  *************/
/**********************************************/

ecs::systems::FleeStateSystem::FleeStateSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(FleeStateComponent::typeID);
	typeFilter.addRequirement(UnitComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
}

ecs::systems::FleeStateSystem::~FleeStateSystem()
{

}

void ecs::systems::FleeStateSystem::updateEntity(FilteredEntity& entity, float delta)
{
	//Initialize variables.
	float distance = 1000.0f;
	//Fetch data of current entity.
	UnitComponent* p_current_unit = entity.getComponent<UnitComponent>();
	TransformComponent* p_current_unit_transform = entity.getComponent<TransformComponent>();
	//Initialize data for other unit.
	UnitComponent* p_other_unit;
	TransformComponent* p_other_unit_transform;
	bool switch_state = true;
	//Get an iterator for every unit alive.
	ComponentIterator it = ECSUser::getComponentsOfType<UnitComponent>();
	while (p_other_unit = static_cast<UnitComponent*>(it.next()))
	{
		//Check so that the unit we are looking at isn't a friendly unit
		if (p_other_unit->playerID != p_current_unit->playerID)
		{
			p_other_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(p_other_unit->getEntityID());
			distance = PhysicsHelpers::CalculateDistance(p_current_unit_transform->position, p_other_unit_transform->position);
			if (distance < TILE_RADIUS * 15)
			{
				switch_state = false;
				break;
			}
		}
	}
	if (switch_state)
	{
		//If the enemy is within attack range switch to a AttackStateComponent
		IdleStateComponent idle_comp;
		idle_comp.activeCommand = FLEE;
		ECSUser::removeComponent(p_current_unit->getEntityID(), FleeStateComponent::typeID);
		ECSUser::createComponent(p_current_unit->getEntityID(), idle_comp);
	}
}

/************************************************/
/**********  FLEESTATESYSTEM END  **************/
/**********************************************/

/************************************************/
/*********  LOOTSTATESYSTEM START  *************/
/**********************************************/

ecs::systems::LootStateSystem::LootStateSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(LootStateComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
	typeFilter.addRequirement(DynamicMovementComponent::typeID);
}

ecs::systems::LootStateSystem::~LootStateSystem()
{

}

void ecs::systems::LootStateSystem::updateEntity(FilteredEntity& entity, float delta)
{
	//ecs::components::TransformComponent* unit_transform = entity.getComponent<ecs::components::TransformComponent>();
	//ecs::components::LootStateComponent* unit_loot_comp = entity.getComponent<ecs::components::LootStateComponent>();
	//ecs::components::DynamicMovementComponent* dynamic_move_comp = entity.getComponent<ecs::components::DynamicMovementComponent>();

	//ecs::components::TransformComponent* loot_tile_transform = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::TransformComponent>(unit_loot_comp->goalID);

	//float dist = PhysicsHelpers::CalculateDistance(unit_transform->position, loot_tile_transform->position);
	//XMFLOAT3 direction;
	//float length;

	//if (dist > 0.1f)
	//{
	//	//Calculate the direction of the enemy and normalize the vector.
	//	direction.x = loot_tile_transform->position.x - unit_transform->position.x;
	//	direction.y = loot_tile_transform->position.y - unit_transform->position.y;
	//	direction.z = loot_tile_transform->position.z - unit_transform->position.z;
	//	length = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	//	direction.x /= length;
	//	direction.y /= length;
	//	direction.z /= length;
	//	//Set the direction.
	//	dynamic_move_comp->mForward = direction;
	//	//Create an event to move the unit towards the enemy.
	//	MovementInputEvent move_event;
	//	move_event.mInput = FORWARD;
	//	move_event.mEntityID = entity.entity->getID();
	//	createEvent(move_event);
	//}
	//else
	//{
	//	ecs::components::PathfindingStateComponent path_comp;
	//	path_comp.activeCommand == LOOT;
	//	ecs::ECSUser::removeComponent(entity.entity->getID(), ecs::components::LootStateComponent::typeID);
	//	ecs::ECSUser::createComponent(entity.entity->getID(), path_comp);
	//}
}

/************************************************/
/**********  LOOTSTATESYSTEM END  **************/
/**********************************************/

/************************************************/
/********  ATTACKSTATESYSTEM START  ************/
/**********************************************/

ecs::systems::AttackStateSystem::AttackStateSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(AttackStateComponent::typeID);
	typeFilter.addRequirement(UnitComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
	typeFilter.addRequirement(EquipmentComponent::typeID);
	typeFilter.addRequirement(DynamicMovementComponent::typeID);
}

ecs::systems::AttackStateSystem::~AttackStateSystem()
{

}

void ecs::systems::AttackStateSystem::updateEntity(FilteredEntity& entity, float delta)
{
	//Fetch data of our current unit.
	TransformComponent* p_current_unit_transform = entity.getComponent<TransformComponent>();
	AttackStateComponent* p_atk_state = entity.getComponent<AttackStateComponent>();
	EquipmentComponent* p_equipment_comp = entity.getComponent<EquipmentComponent>();

	//Enemy unit data
	Entity* p_enemy_entity = ECSUser::getEntity(p_atk_state->enemyEntityId);
	TransformComponent* p_enemy_unit_transform;

	//Distance to the enemy unit.
	float distance;
	bool remove_component = false;
	//Check if the enemy unit still exists
	if (p_enemy_entity)
	{
		//Fetch the enemy units data
		p_enemy_unit_transform = ECSUser::getComponentFromKnownEntity<TransformComponent>(p_enemy_entity->getID());
		//Calculate distance to the enemy unit
		distance = PhysicsHelpers::CalculateDistance(p_current_unit_transform->position, p_enemy_unit_transform->position);
		//If the enemy is not within attack range remove attack component
		if (distance > p_equipment_comp->mAttackRange * 1.5f)
		{
			remove_component = true;
		}
	}
	else //if the enemy doesn't exist anymore, change state.
	{
		remove_component = true;
	}
	if (remove_component)
	{
		if (p_atk_state->activeCommand == IDLE)
		{
			IdleStateComponent idle_comp;
			idle_comp.activeCommand = IDLE;
			ECSUser::createComponent(p_current_unit_transform->getEntityID(), idle_comp);
		}
		ECSUser::removeComponent(p_current_unit_transform->getEntityID(), AttackStateComponent::typeID);
	}
}

/************************************************/
/*********  ATTACKSTATESYSTEM END  *************/
/**********************************************/

/************************************************/
/******  REMOVEDEADUNITSSYSTEM START  **********/
/**********************************************/

ecs::systems::RemoveDeadUnitsSystem::RemoveDeadUnitsSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(DeadComponent::typeID);
}

ecs::systems::RemoveDeadUnitsSystem::~RemoveDeadUnitsSystem()
{

}

void ecs::systems::RemoveDeadUnitsSystem::updateEntity(FilteredEntity& entity, float delta)
{
	// DEATH EFFECTS	
	DeadComponent* p_dead = getComponentFromKnownEntity<DeadComponent>(entity.entity->getID());
	if (p_dead->cause == DeadComponent::CAUSE_DROWNING)
	{
		// Splash Emitter - When drowned, spawn a water splash	
		components::ParticleSpawnerComponent spawner;
		components::SplashSpawnerComponent smoke;

		spawner.StartPosition = p_dead->position;
		spawner.SpawnFrequency = 0.005f;
		spawner.TimerSinceLastSpawn = 0.0f;
		spawner.LifeDuration = 1.0f;

		smoke.InitialVelocity = 8.0f;
		smoke.SpawnCount = 100;

		createEntity(spawner, smoke);

		// Doesnt work atm	
		/*PlaySound sound;
		sound.audioName = SPLOOSH_SOUND;
		sound.soundFlags = SF_NONE;
		ecs::ECSUser::createEvent(sound);*/
	}
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
	EquipmentComponent* equipment_comp = ECSUser::getComponentFromKnownEntity<EquipmentComponent>(entity.entity->getID());
	Entity* weapon_entity = ECSUser::getEntity(equipment_comp->mEquippedWeapon);
	WeaponComponent* weapon_comp = ECSUser::getComponentFromKnownEntity<WeaponComponent>(equipment_comp->mEquippedWeapon);
	//Remove the weapon entity if the weapon is a FIST else set the owner of the weapon to 0 so that another unit can pick it up.
	if (weapon_comp->mType == GAME_OBJECT_TYPE_WEAPON_FIST)
	{
		ECSUser::removeEntity(weapon_entity->getID());
	}
	else
	{
		ECSUser::removeEntity(weapon_entity->getID());
		//weapon_comp->mOwnerEntity = 0;
	}
	//Remove the dead unit
	ECSUser::removeEntity(entity.entity->getID());
}

/************************************************/
/*******  REMOVEDEADUNITSSYSTEM END  ***********/
/**********************************************/

/************************************************/
/*********  SWITCHSTATESYSTEM START  ***********/
/**********************************************/

ecs::systems::SwitchStateSystem::SwitchStateSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::ChangeUserStateEvent::typeID);
}

ecs::systems::SwitchStateSystem::~SwitchStateSystem()
{

}

void ecs::systems::SwitchStateSystem::readEvent(BaseEvent& event, float delta)
{
	if (event.getTypeID() == ChangeUserStateEvent::typeID)
	{
		//Save values for easier use.
		int player = static_cast<ChangeUserStateEvent*>(&event)->playerId;
		STATE state = static_cast<ChangeUserStateEvent*>(&event)->newState;
		//Find the correct player for the event.
		int i = 0;
		ComponentIterator it = ECSUser::getComponentsOfType(ArmyComponent::typeID);
		ArmyComponent* p_army = static_cast<ArmyComponent*>(it.next());
		while (i < player)
		{
			p_army = static_cast<ArmyComponent*>(it.next());
			i++;
		}

		// Chagne the command on the UI
		BaseComponent* p_base_comp = getComponentFromKnownEntity<UITextComponent>(p_army->getEntityID());

		/*UITextComponent* p_text_comp = static_cast<UITextComponent*>(p_base_comp);

		switch (state)
		{
		case STATE::IDLE:
			p_text_comp->mStrText = L"IDLE";
			break;
		case STATE::LOOT:
			p_text_comp->mStrText = L"LOOT";
			break;
		case STATE::ATTACK:
			p_text_comp->mStrText = L"ATTACK";
			break;
		case STATE::FLEE:
			p_text_comp->mStrText = L"FLEE";
			break;
		default:
			p_text_comp->mStrText = L"no case for state";
			break;
		}
*/
		//Loop through the players units and remove their old state component.
		Entity* unit;
		for (int u = 0; u < p_army->unitIDs.size(); u++)
		{
			ID entity_id = p_army->unitIDs[u];
			unit = ECSUser::getEntity(entity_id);

			ECSUser::removeComponent(entity_id, PathfindingStateComponent::typeID);
			ECSUser::removeComponent(entity_id, IdleStateComponent::typeID);
			ECSUser::removeComponent(entity_id, MoveStateComponent::typeID);
			ECSUser::removeComponent(entity_id, FleeStateComponent::typeID);
			ECSUser::removeComponent(entity_id, LootStateComponent::typeID);
			ECSUser::removeComponent(entity_id, AttackStateComponent::typeID);

			// Fetch the skeleton ID to start animations based on state
			ID skeleton_id = unit->getComponentID<SkeletonComponent>();

			//Give the unit the new state component.
			switch (state)
			{
			case STATE::IDLE:
			{
				IdleStateComponent idle;
				ECSUser::createComponent(entity_id, idle);
				break;
			}
			default:
			{
				//Defaults to a path since every other command relies on moving to a destination
				PathfindingStateComponent path;
				path.activeCommand = state;
				ECSUser::createComponent(entity_id, path);
				break;
			}
			}
			/*Used for debugging*/
			//std::cout << "Changing state of player: " << player << " which has the entityID: " << p_army->getEntityID() << std::endl;
		}
	}
}

/************************************************/
/*********  SWITCHSTATESYSTEM END  *************/
/**********************************************/

/************************************************/
/*****  POTENTIALWATERHAZARDSYSTEM START  ******/
/**********************************************/

ecs::systems::PotentialWaterHazardSystem::PotentialWaterHazardSystem()
{
	updateType = MultiEntityUpdate;
	typeFilter.addRequirement(components::TileComponent::typeID);
	typeFilter.addRequirement(components::TransformComponent::typeID);
}

ecs::systems::PotentialWaterHazardSystem::~PotentialWaterHazardSystem()
{

}

void ecs::systems::PotentialWaterHazardSystem::updateMultipleEntities(EntityIterator& entities, float delta)
{
	for (FilteredEntity& entity : entities.entities)
	{
		//Fetch relevant components from the current tile.
		components::TileComponent* current_tile_comp = entity.getComponent<components::TileComponent>();
		components::TransformComponent* current_tile_transform = entity.getComponent<components::TransformComponent>();
		//Filter out all of the tiles in the world.
		ecs::TypeFilter tile_filter;
		tile_filter.addRequirement(components::TileComponent::typeID);
		tile_filter.addRequirement(components::TransformComponent::typeID);
		ecs::EntityIterator e_it = ecs::ECSUser::getEntitiesByFilter(tile_filter);
		//Distance variable used to calculate the hazards charge of the current tile based on distance to other water tiles.
		float distance;
		//Check if the current tile is a water tile. If so we want to give it a very high hazards value.
		if (current_tile_comp->tileType != WATER)
		{
			//Loop through every tile in the world and calculate its hazard impact on the current tile
			for (FilteredEntity& tile : e_it.entities)
			{
				//Skip the tile if it is the current tile we are looking at or if the tile is not a water tile
				if (current_tile_comp->getEntityID() != tile.entity->getID() && tile.getComponent<components::TileComponent>()->tileType == WATER)
				{
					XMFLOAT3 dist;
					dist.x = current_tile_transform->position.x - tile.getComponent<TransformComponent>()->position.x;
					dist.y = 0.0f;
					dist.z = current_tile_transform->position.z - tile.getComponent<TransformComponent>()->position.z;
					distance = sqrt(dist.x * dist.x + dist.z * dist.z);
					if (distance < TILE_RADIUS * 5)
					{
						current_tile_comp->charges.hazardCharge += 10.0f / (distance * distance);
					}
				}
			}
		}
		else
		{
			current_tile_comp->charges.hazardCharge = 100.0f; //If this is a water tile we want it to be the worst possible option always.
		}
	}
	ecs::ECSUser::RemoveSystem<PotentialWaterHazardSystem>();
}

/************************************************/
/****** POTENTIALWATERHAZARDSYSTEM END  ********/
/**********************************************/

/************************************************/
/******  POTENTIALARMYHAZARDSYSTEM START  ******/
/**********************************************/

ecs::systems::PotentialArmyHazardSystem::PotentialArmyHazardSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(components::TileComponent::typeID);
	typeFilter.addRequirement(components::TransformComponent::typeID);
}

ecs::systems::PotentialArmyHazardSystem::~PotentialArmyHazardSystem()
{

}

void ecs::systems::PotentialArmyHazardSystem::updateEntity(FilteredEntity& entity, float delta)
{
	//Get relevant components of the current tile.
	components::TileComponent* current_tile_comp = entity.getComponent<components::TileComponent>();
	components::TransformComponent* current_tile_transform = entity.getComponent<components::TransformComponent>();
	//Fetch the army components
	ecs::ComponentIterator c_it = ecs::ECSUser::getComponentsOfType<components::ArmyComponent>();
	ecs::components::ArmyComponent* current_army;
	std::vector<components::ArmyComponent*> armies;
	//Save the armies in a vector for easier access in the next loop.
	while (current_army = static_cast<components::ArmyComponent*>(c_it.next()))
	{
		armies.push_back(current_army);
	}
	//Distance variable used to calculate the charge.
	float distance;
	if (current_tile_comp->tileType != WATER)
	{
		//Reset charges from last pass.
		for (int i = 0; i < 4; i++)
		{
			current_tile_comp->charges.armyCharges[i] = 0.0f;
		}
		//Calculate each armies hazards factor on this tile based on their distance to it.
		for (int a = 0; a < armies.size(); a++)
		{
			//For each unit of the current army calculate their hazard impact on this tile and save it in their charge variable in the tile component.
			for (int u = 0; u < armies[a]->unitIDs.size(); u++)
			{
				XMFLOAT3 dist;
				dist.x = current_tile_transform->position.x - ECSUser::getComponentFromKnownEntity<TransformComponent>(armies[a]->unitIDs[u])->position.x;
				dist.y = 0.0f;
				dist.z = current_tile_transform->position.z - ECSUser::getComponentFromKnownEntity<TransformComponent>(armies[a]->unitIDs[u])->position.z;
				distance = sqrt(dist.x * dist.x + dist.z * dist.z);
				//distance = PhysicsHelpers::CalculateDistance(current_tile_transform->position, ecs::ECSUser::getComponentFromKnownEntity<components::TransformComponent>(armies[a]->unitIDs[u])->position);
				current_tile_comp->charges.armyCharges[a] += 60.0f / (distance * distance);
			}
		}

		//DEBUG CODE//
		//LETS THE DEVELOPER SEE THE POTENTIAL FIELD AS RED COLOR ON THE TILES.
		//THE DARKER THE TILE THE BETTER THAT TILE IS FOR THE RED ARMY TO FLEE TO!
		/*
		ecs::components::ColorComponent* p_color_comp = ecs::ECSUser::getComponentFromKnownEntity<components::ColorComponent>(entity.entity->getID());
		p_color_comp->blue = 0.0f;
		p_color_comp->green = 0.0f;
		float charge = (current_tile_comp->charges.armyCharges[1] + current_tile_comp->charges.armyCharges[2] + current_tile_comp->charges.armyCharges[3] + current_tile_comp->charges.hazardCharge) / 255.0f;
		if (charge > 1.0f)
		{
			charge = 255.0f;
		}
		else
		{
			charge *= 255.0f;
		}
		p_color_comp->red = charge;
		*/
		//END OF DEBUG CODE//
	}
	else
	{
		//If this is a water tile we set the army hazards to 100 so that no one ever will want to walk on it.
		for (int i = 0; i < 4; i++)
		{
			current_tile_comp->charges.armyCharges[i] = 100.0f;
		}
	}
}

/************************************************/
/*******  POTENTIALARMYHAZARDSYSTEM END  *******/
/**********************************************/