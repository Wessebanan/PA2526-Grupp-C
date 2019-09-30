#include "AIFunctions.h"
#include "AIGlobals.h"
#include "AIComponents.h"
#include "UtilityComponents.h"
#include "GridFunctions.h"

namespace AIFunctions
{
	//Creates and spawns in the army of every player in the arena. Must run AFTER the arena
	//has been created.
	void CreatePlayerArmies(ecs::EntityComponentSystem& rEcs)
	{
		//Create Components for a "User" entity.
		ecs::components::ArmyComponent army;
		//Create Components for a "Unit" entity.
		ecs::components::TransformComponent transform;
		ecs::components::UnitComponent unit;
		ecs::components::IdleStateComponent idle_state;
		//Temporary entity pointer so that we can fetch the units IDs so that we can store
		//them in the army component.
		ecs::Entity* temp_entity;
		int2 starting_tile_index;
		ID temp_id;
		ecs::components::TransformComponent* p_transform;
		//Loop for every player.
		for (int i = 0; i < 4; i++)
		{
			//Fetch the index of the starting tile for this player.
			starting_tile_index = GridFunctions::FindStartingTile((PLAYER)i);
			temp_id = ArenaProperties::gridLogic[starting_tile_index.y][starting_tile_index.x].entityID;
//			p_transform = rEcs.getComponentFromEntity<ecs::components::TransformComponent>(temp_id);
			//Set current players enum ID for this armies units.
			unit.playerID = (PLAYER)i;

			for (int u = 0; u < PlayerProperties::numberOfUnits; u++)
			{
				//Set starting position of the unit.
//				transform.position.x = p_transform->position.x;
//				transform.position.y = p_transform->position.y + 50;
//				transform.position.z = p_transform->position.z;
				temp_entity = rEcs.createEntity(transform, unit, idle_state);
				army.unitIDs.push_back(temp_entity->getID());
			}
			//Create the user entity
			rEcs.createEntity(army);
			//Clear the army vector before we start creating the next players army.
			army.unitIDs.clear(); 
		}
		int dank = 0;
	}
}