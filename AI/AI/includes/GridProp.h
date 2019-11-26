#pragma once
#include <vector>
#include "AIGlobals.h"
//#include "../../ECS/plainECS/includes/ecsGlobals.h"
#include "..//../..//ECS/plainECS/includes/ecsGlobals.h"

struct TileData
{
	unsigned int Id;
	bool isPassable;
	float height;
	unsigned int neighbourIDArray[6] = { 0 };
	int biome = -1;
};

class GridProp
{
private:
	GridProp();
	~GridProp();

	// Stop the compiler generating methods of copy the object
	GridProp(GridProp const& copy) = delete;
	GridProp& operator = (GridProp const& copy) = delete;

	int2 mMapSize = int2(0,0);

	// Singleton
	static GridProp* mInstance;

public:
	TileData mGrid[MAX_ARENA_COLUMNS][MAX_ARENA_ROWS];
	int mSceneObjects[MAX_ARENA_COLUMNS][MAX_ARENA_ROWS];
	int mCurrentMap = -1;
	std::vector<ID> mLootTiles;

	int2 GetSize();
	void SetSize(int rows, int columns);

	// Singleton
	static GridProp* GetInstance();
};
