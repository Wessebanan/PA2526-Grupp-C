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

#include "..//gameGameLoop/GameLoopEvents.h"

namespace AIEcsFunctions
{
	// Creates a army component of each player
	void CreatePlayers(ecs::EntityComponentSystem& rEcs)
	{
		for (size_t i = 0; i < 4; i++)
		{
			ecs::components::ArmyComponent army;
			
			army.playerID = (PLAYER)i;

			//Create the user entity
			rEcs.createEntity(army);
		}
	}

	void CreateUnits(ecs::EntityComponentSystem& rEcs)
	{
		//ecs::events::GameStartEvent eve;
		//rEcs.createEvent(eve);
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