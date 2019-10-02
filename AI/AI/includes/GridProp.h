#pragma once
#include "AIGlobals.h"

struct TileData
{
	unsigned int Id;
	bool isPassable;
	float height;
};

class GridProp
{
private:
	GridProp() {};
	static GridProp* mpInstance;
	// Stop the compiler generating methods of copy the object
	GridProp(GridProp const& copy);            // Not Implemented
	GridProp& operator = (GridProp const& copy); // Not Implemented
	~GridProp() {};
public:
	TileData mGrid[ARENA_COLUMNS][ARENA_ROWS];
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
