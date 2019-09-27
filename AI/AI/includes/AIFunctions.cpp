#include "AIFunctions.h"
#include "AIGlobals.h"
#include "AIComponents.h"
#include "UtilityComponents.h"

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

		//Loop for every player.
		for (int i = 0; i < 4; i++)
		{
			//Set current players enum ID for this armies units.
			unit.playerID = (PLAYER)i;
			for (int u = 0; u < PlayerProperties::numberOfUnits; u++)
			{
				//Set starting position of the unit.
				transform.position.x;
				transform.position.y;
				transform.position.z;
				temp_entity = rEcs.createEntity(transform, unit, idle_state);
				army.unitIDs.push_back(temp_entity->getID());
			}
			//Create the user entity
			rEcs.createEntity(army);
			//Clear the army vector before we start creating the next players army.
			army.unitIDs.clear(); 
		}
	}
}