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
	static GridProp* mpInstance;
	// Stop the compiler generating methods of copy the object
	GridProp(GridProp const& copy);            // Not Implemented
	GridProp& operator = (GridProp const& copy); // Not Implemented
	~GridProp() {};
public:
	TileData mGrid[ARENA_COLUMNS][ARENA_ROWS];
	int mSceneObjects[ARENA_COLUMNS][ARENA_ROWS];

	int2 GetSize() { return this->mMapSize; };
	void SetSize(int rows, int columns) { this->mMapSize.x = rows; this->mMapSize.y = columns; };
	static GridProp* GetInstance()
	{
		{
			if (mpInstance == 0)
			{
				mpInstance = new GridProp;
			}
			return mpInstance;
		}
	}

	static void Destroy()
	{
		if (!mpInstance)
		{
			delete mpInstance;
		}
	}
};
