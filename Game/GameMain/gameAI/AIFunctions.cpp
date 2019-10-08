#include "AIFunctions.h"
#include "GridProp.h"
#include "AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
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
		GridProp* p_gp = GridProp::GetInstance();
		//Loop for every player.
		for (int i = 0; i < 4; i++)
		{
			////Fetch the index of the starting tile for this player.
			starting_tile_index = GridFunctions::FindStartingTile((PLAYER)i);
			temp_id = p_gp->mGrid[starting_tile_index.y][starting_tile_index.x].Id;
			p_transform = rEcs.getComponentFromEntity<ecs::components::TransformComponent>(temp_id);
			//Set current players enum ID for this armies units.
			unit.playerID = (PLAYER)i;

			for (int u = 0; u < PlayerProperties::numberOfUnits; u++)
			{
				//Set starting position of the unit.
				transform.position.x = p_transform->position.x + u * 0.2f;
				transform.position.y = p_transform->position.y + 2.0f;
				transform.position.z = p_transform->position.z + u * 0.3f;

				// set scale to fit on tile
				transform.scale.x = 0.1f;
				transform.scale.y = 0.1f;
				transform.scale.z = 0.1f;

				// roate them 90deg
				transform.rotation.x = -1.57079633f;
				switch (i)
				{
				case 0:
					transform.rotation.y = (1.57079633f / 2.0f);
					break;
				case 1:
					transform.rotation.y = (-1.57079633f * 1) + (1.57079633f / 2.0f);
					break;
				case 2:
					transform.rotation.y = (1.57079633f * 1) + (1.57079633f / 2.0f);
					break;
				case 3:
					transform.rotation.y = (1.57079633f * 2) + (1.57079633f / 2.0f);
					break;
				default:
					transform.rotation.y = (1.57079633f * i) + (1.57079633f / 2.0f);
					break;
				}


				temp_entity = rEcs.createEntity(transform, unit, idle_state); //
				army.unitIDs.push_back(temp_entity->getID());
			}
			//Create the user entity
			rEcs.createEntity(army);
			//Clear the army vector before we start creating the next players army.
			army.unitIDs.clear(); 
		}
	}
}