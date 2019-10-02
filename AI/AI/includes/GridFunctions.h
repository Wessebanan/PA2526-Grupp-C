#pragma once
#include <ecs.h>
#include "AIComponents.h"
#include "AISystems.h"
#include "AIEvents.h"
#include <DirectXMath.h>

namespace GridFunctions
{
	//Calculates the centerposition of all the tiles in the grid and creates them.
	void CreateGrid(ecs::EntityComponentSystem& rEcs, const int rows, const int columns, const float radius);
	//Create a system for printing every tiles center position for debugging purposes.
	void CreateDebugSystems(ecs::EntityComponentSystem& rEcs);
	void CreateHeightmap(float* arr);
	void CreatePotentialField(ecs::EntityComponentSystem& rEcs);
	float CreateCharge(float startX, float startZ, float endX, float endZ, float charge);
	float getDistance(float startX, float startZ, float endX, float endZ);
	bool CheckIfValidNeighbour(int2 currentTile, int2 neighbourIndex);
	void StoreNeighbours();
	DirectX::XMFLOAT2 FindStartingTile(PLAYER id);
};