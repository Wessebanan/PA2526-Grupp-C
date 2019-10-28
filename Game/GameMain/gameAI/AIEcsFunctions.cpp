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

	//Creates and spawns in the army of every player in the arena. Must run AFTER the arena
	//has been created.
	void CreateUnits(ecs::EntityComponentSystem& rEcs)
	{
		ecs::events::GameStartEvent eve;
		rEcs.createEvent(eve);
	}
}