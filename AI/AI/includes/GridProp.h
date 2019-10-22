#pragma once
#include "AIGlobals.h"

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
	GridProp() {};
	// Stop the compiler generating methods of copy the object
	GridProp(GridProp const& copy);            // Not Implemented
	GridProp& operator = (GridProp const& copy); // Not Implemented
	~GridProp() {};

public:
	TileData mGrid[ARENA_COLUMNS][ARENA_ROWS];
	int mSceneObjects[ARENA_COLUMNS][ARENA_ROWS];

	static GridProp* GetInstance()
	{
		static GridProp instance;
		return &instance;
	}
};
