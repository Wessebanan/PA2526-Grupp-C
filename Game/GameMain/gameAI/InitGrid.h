#pragma once

#include "ecs.h"

#include "../../AI/AI/includes/GridProp.h"
#include "..//gameAI/GridEcsFunctions.h"
#include "..//gameAI/AIComponents.h"
#include "..//gameUtility/UtilityComponents.h"
#include "../gameSceneObjects/InitBiomes.h"
#include <DirectXMath.h>


void InitGrid(ecs::EntityComponentSystem& rECS)
{
	GridProp* p_gp = GridProp::GetInstance();
	int random_map;

	int rows = 0;
	int columns = 0;
	bool holmes = true;
	

	if (p_gp->mCurrentMap > -1)
	{
		random_map = p_gp->mCurrentMap;
	}
	else
	{
		random_map = rand() % 3;
		p_gp->mCurrentMap = random_map;


	}

	// 3 preset mapsizes
	switch (random_map)
	{
	case 0:
		rows = 15;
		columns = 15;
		break;
	case 1:
		rows = 22;
		columns = 22;
		break;
	case 2:
		rows = 28;
		columns = 28;
		break;
	default:
		// For testing the map can be set to close to maximum
		rows = MAX_ARENA_ROWS - 3;
		columns = MAX_ARENA_COLUMNS - 3;
		break;
	}

	GridEcsFunctions::CreateGrid(rECS, rows, columns, 1.0f, holmes);
	InitBiomes(rECS, rows, columns);
}
