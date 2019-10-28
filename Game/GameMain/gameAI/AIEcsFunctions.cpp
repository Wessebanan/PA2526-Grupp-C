#include "AIEcsFunctions.h"
#include "../../AI/includes/GridProp.h"
#include "AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "GridEcsFunctions.h"
#include "../../AI/includes/GridFunctions.h"
#include "../gameAnimation/AnimationComponents.h"
#include "../MeshContainer/MeshContainer.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

namespace AIEcsFunctions
{
	//Creates and spawns in the army of every player in the arena. Must run AFTER the arena
	//has been created.
	void CreatePlayerArmies(ecs::EntityComponentSystem& rEcs)
	{
		/* TEAM COLORS */
		struct uint3
		{
			UINT r, g, b;
		};

		uint3 army_colors[4];

		army_colors[0] = { 200,   0,   0 };	// Red		Army 1
		army_colors[1] = {  20,  20,  20 };	// Gray		Army 2
		army_colors[2] = {   0, 100, 100 };	// Cyan		Army 3
		army_colors[3] = { 100,   0, 100 };	// Purple	Army 4

		/* END	*/


		//Set seed
		std::srand(std::time(nullptr));
		//Create Components for a "User" entity.
		ecs::components::ArmyComponent army;
		//Create Components for a "Unit" entity.
		ecs::components::TransformComponent transform;
		ecs::components::UnitComponent unit;
		ecs::components::IdleStateComponent idle_state;
		ecs::components::ColorComponent color_comp;
		ecs::components::SkeletonComponent skeleton_comp;
		//Temporary entity pointer so that we can fetch the units IDs so that we can store
		//them in the army component.
		ecs::Entity* temp_entity;
		int2 starting_tile_index;
		TypeID temp_id = 0;
		ecs::components::TransformComponent* p_transform;
		GridProp* p_gp = GridProp::GetInstance();
		int2 size = p_gp->GetSize();
		//Loop for every player.
		for (int i = 0; i < 4; i++)
		{
			////Fetch the index of the starting tile for this player.
			starting_tile_index = GridFunctions::FindStartingTile((PLAYER)i, size.x,size.y, MAPINITSETTING::NOHOLMES);
			temp_id = p_gp->mGrid[starting_tile_index.y][starting_tile_index.x].Id;
			if (temp_id)
			{
				p_transform = rEcs.getComponentFromEntity<ecs::components::TransformComponent>(temp_id);
				//Set current players enum ID for this armies units.
				unit.playerID = (PLAYER)i;
				int divider = 3;
				for (int u = 0; u < PlayerProperties::numberOfUnits; u++)
				{
					//Set the starting position of the unit depending on the center position of the units starting tile. Needs to
					//be updated if the number of units is increased beyond 3.
					if (u == 0)
					{
						transform.position.x = p_transform->position.x + (float(TILE_RADIUS) / divider);
						transform.position.y = p_transform->position.y + 1.1f;
						transform.position.z = p_transform->position.z + (float(TILE_RADIUS) / divider);
					}
					else if (u == 1)
					{
						transform.position.x = p_transform->position.x - (float(TILE_RADIUS) / divider);
						transform.position.y = p_transform->position.y + 1.1f;
						transform.position.z = p_transform->position.z + (float(TILE_RADIUS) / divider);
					}
					else
					{
						transform.position.x = p_transform->position.x;
						transform.position.y = p_transform->position.y + 1.1f;
						transform.position.z = p_transform->position.z - (float(TILE_RADIUS) / divider);
					}

					// set scale to fit on tile
					transform.scale.x = 0.1f;
					transform.scale.y = 0.1f;
					transform.scale.z = 0.1f;

					color_comp.red = army_colors[i].r;
					color_comp.green = army_colors[i].g;
					color_comp.blue = army_colors[i].b;

					temp_entity = rEcs.createEntity(transform, unit, idle_state, color_comp, skeleton_comp); //
					army.unitIDs.push_back(temp_entity->getID());
			}
			
				
			}
			//Create the user entity
			rEcs.createEntity(army);
		//	//Clear the army vector before we start creating the next players army.
			std::cout << "Player " << i << " army ids: ";
			for (int k = 0; k < army.unitIDs.size(); k++)
			{
				std::cout << army.unitIDs[k] << ", ";
			}
			std::cout << std::endl;
			army.unitIDs.clear(); 
		}
	}
}