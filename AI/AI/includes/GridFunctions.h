#pragma once
#include "AIGlobals.h"
#include <DirectXMath.h>

namespace GridFunctions
{
	void CreateHeightmap(float* Arr);
	float CreateCharge(float StartX, float StartZ, float EndX, float EndZ, float Charge);
	float GetDistance(float startX, float startZ, float endX, float endZ);
	bool CheckIfValidNeighbour(int2 currentTile, int2 neighbourIndex);
	void StoreNeighbours();
	//Returns the index of the starting tile for the given player.
	int2 FindStartingTile(PLAYER Id);
	
};