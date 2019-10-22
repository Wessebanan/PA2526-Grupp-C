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
	int rows = 14;
	int columns = 14;

	GridEcsFunctions::CreateGrid(rECS, rows, columns, 1.0f);
	InitBiomes(rECS, rows, columns);
}