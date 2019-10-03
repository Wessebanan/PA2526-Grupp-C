#pragma once
#include <ecs.h>
#include "AIComponents.h"
#include "AISystems.h"
#include "AIEvents.h"
#include <DirectXMath.h>

namespace GridFunctions
{
	//Calculates the centerposition of all the tiles in the grid and creates them.
	void CreateGrid(ecs::EntityComponentSystem& rEcs, const int Rows, const int Columns, const float Radius);
	//Create a system for printing every tiles center position for debugging purposes.
	void CreateDebugSystems(ecs::EntityComponentSystem& rEcs);
	void CreateHeightmap(float* Arr);
	void CreatePotentialField(ecs::EntityComponentSystem& rEcs);
	float CreateCharge(float StartX, float StartZ, float EndX, float EndZ, float Charge);
	//Returns the index of the starting tile for the given player.
	int2 FindStartingTile(PLAYER Id);
	float CreateCharge(float startX, float startZ, float endX, float endZ, float charge);
	float GetDistance(float startX, float startZ, float endX, float endZ);
	bool CheckIfValidNeighbour(int2 currentTile, int2 neighbourIndex);
	std::vector<unsigned int> FindPath(ecs::EntityComponentSystem& rEcs, unsigned int startID, unsigned int endID);
	unsigned int FindGoalTile(ecs::EntityComponentSystem& rEcs);
	void StoreNeighbours(ecs::EntityComponentSystem& rEcs);
};