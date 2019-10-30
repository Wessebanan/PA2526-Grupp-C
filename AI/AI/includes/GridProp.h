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
	int2 mMapSize = int2(0,0);
	// Stop the compiler generating methods of copy the object
	GridProp(GridProp const& copy);            // Not Implemented
	GridProp& operator = (GridProp const& copy); // Not Implemented
	~GridProp() {};

public:
	TileData mGrid[MAX_ARENA_COLUMNS][MAX_ARENA_ROWS];
	int mSceneObjects[MAX_ARENA_COLUMNS][MAX_ARENA_ROWS];
	int mCurrentMap = -1;
	std::vector<ID> mLootTiles;
	int2 GetSize() { return this->mMapSize; };
	void SetSize(int rows, int columns) { this->mMapSize.x = rows; this->mMapSize.y = columns; };
	static GridProp* GetInstance()
	{
		static GridProp instance;
		return &instance;
	}
};
