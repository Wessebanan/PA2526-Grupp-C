#pragma once
#include <ecs.h>
#include "AIComponents.h"
#include "AISystems.h"
#include "AIEvents.h"
#include <DirectXMath.h>

namespace GridEcsFunctions
{
	//Calculates the centerposition of all the tiles in the grid and creates them.
	void CreateGrid(ecs::EntityComponentSystem& rEcs, const int Rows, const int Columns, const float Radius);
	//Create a system for printing every tiles center position for debugging purposes.
	void CreateDebugSystems(ecs::EntityComponentSystem& rEcs);
	void CreatePotentialField(ecs::EntityComponentSystem& rEcs);
	std::vector<unsigned int> FindPath(ecs::EntityComponentSystem& rEcs, unsigned int startID, unsigned int endID);
};