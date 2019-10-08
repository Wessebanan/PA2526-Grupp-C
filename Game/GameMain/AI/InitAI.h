#pragma once

#include "AIFunctions.h"
#include "GridFunctions.h"
#include "ecs.h"

void InitAI(ecs::EntityComponentSystem& rECS)
{
	unsigned int grid_width = 12;
	unsigned int grid_height = 12;
	unsigned int nr_tiles = grid_height * grid_width;

	GridFunctions::CreateGrid(rECS, grid_width, grid_height, 1.0f);
}