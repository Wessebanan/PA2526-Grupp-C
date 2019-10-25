#pragma once
#include "AIGlobals.h"
#include <DirectXMath.h>

namespace GridFunctions
{
	void CreateHeightmap(float* Arr, int rows, int colums, float height_power, int mountains);
	float CreateCharge(float StartX, float StartZ, float EndX, float EndZ, float Charge);
	float GetDistance(float startX, float startZ, float endX, float endZ);
	bool CheckIfValidNeighbour(int2 currentTile, int2 neighbourIndex);
	void StoreNeighbours();
	//Returns the index of the starting tile for the given player.
	int2 FindStartingTile(PLAYER Id, int Rows, int Columns, MAPINITSETTING Setting);
	int2 GetTileFromWorldPos(float x, float z);
};