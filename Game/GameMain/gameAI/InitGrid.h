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
	int rows = (rand() % (MAX_ARENA_ROWS - 9));
	int columns = (rand() % (MAX_ARENA_COLUMNS - 9));

	rows += 8;
	columns += 8;


	GridEcsFunctions::CreateGrid(rECS, rows, columns, 1.0f);
	InitBiomes(rECS, rows, columns);
}


/*
	int random_map = rand() % 3;

	int rows = 4;
	int columns = 4;

	switch (random_map)
	{
	case 0:
		rows = 12;
		columns = 12;
		break;
	case 1:
		rows = 20;
		columns = 20;
		break;
	case 2:
		rows = 24;
		columns = 20;
		break;
	default:
		break;
	}

*/