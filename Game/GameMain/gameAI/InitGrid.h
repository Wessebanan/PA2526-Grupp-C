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
	int random_map = rand() % 3;

	int rows = 0;
	int columns = 0;

	// 3 preset mapsizes

	switch (random_map)
	{
	case 0:
		rows = 12;
		columns = 12;
		break;
	case 1:
		rows = 18;
		columns = 18;
		break;
	case 2:
		rows = 20;
		columns = 24;
		break;
	default:
		// For testing the map can be set to close to maximum
		rows = MAX_ARENA_ROWS - 3;
		columns = MAX_ARENA_COLUMNS - 3;
		break;
	}



	GridEcsFunctions::CreateGrid(rECS, rows, columns, 1.0f);
	InitBiomes(rECS, rows, columns);
}
