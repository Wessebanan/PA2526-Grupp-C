#pragma once
#include <ecs.h>
#include "AIComponents.h"
#include "AISystems.h"
#include "AIEvents.h"

namespace GridFunctions
{
	//Calculates the centerposition of all the tiles in the grid and creates them.
	void CreateGrid(ecs::EntityComponentSystem& rEcs, const int rows, const int columns, const float radius);
	//Create a system for printing every tiles center position for debugging purposes.
	void CreateDebugSystems(ecs::EntityComponentSystem& rEcs);
	void CreateHeightmap(float* arr);
};