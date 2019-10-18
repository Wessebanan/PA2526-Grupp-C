#include "GridFunctions.h"
#include "GridProp.h"
#include <DirectXMath.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace DirectX;

namespace GridFunctions
{
	void CreateHeightmap(float* Arr) //Creates a fixed array that is used to change the hight for the map
		// size is 12x12 this will be changed in the future if creation of dynamic map size is desired 
	{
		float height_values[144] =
		{ 0.f,0.f,0.f,0.f,1.f,1.f,1.f,0.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,0.f,0.f,1.f,1.f,1.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
		  -1.f,-1.f,-1.f,-1.f,-1.f,0.f,0.f,-1.f,-1.f,-1.f,-1.f,-1.f,
		  0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,0.f,2.f,0.f,0.f,
		  0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,0.f,2.f,0.f,0.f,
		  0.f,1.f,2.f,3.f,2.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };

		for (int i = 0; i < 144; i++)
		{
			Arr[i] = height_values[i];
		}
	}

	float CreateCharge(float StartX, float StartZ, float EndX, float EndZ, float Charge)
	{
		float to_return = 0.f;
		float x = fabsf(EndX - StartX);
		float z = fabsf(EndZ - StartZ);
		float dist = sqrt(x * x + z * z);//get the distance from start to end
		dist = dist / ((TILE_RADIUS)*4);//scale the distance for better values
		int sign = (int)(fabs(Charge) / Charge);//get the sign from charge variable "+" or "-"
		to_return = sign*pow(fabs(Charge), 1 / (dist + 1.2f));//return a exponentially decreasing value depending on distance

		return to_return;
	}

	float GetDistance(float startX, float startZ, float endX, float endZ)
	{
		float to_return = 0.f;
		float x = fabsf(endX - startX);
		float z = fabsf(endZ - startZ);
		float dist = sqrt(x * x + z * z);//get the distance from start to end
		to_return = dist / ((TILE_RADIUS)*2.f);//scale the distance for better values
		return to_return;
	}

	bool CheckIfValidNeighbour(int2 currentTile, int2 neighbourIndex)
	{
		GridProp* p_gp = GridProp::GetInstance();
		bool returnValue = false;
		//Check if the given index is a valid index and check so that the height difference between the tiles is not to large.
		if (neighbourIndex.x >= 0 && neighbourIndex.x < ARENA_COLUMNS
			&& neighbourIndex.y >= 0 && neighbourIndex.y < ARENA_ROWS
			&& p_gp->mGrid[currentTile.x][currentTile.y].height -
			p_gp->mGrid[neighbourIndex.x][neighbourIndex.y].height >= -1)
			returnValue = true;

		return returnValue;
	}

	void StoreNeighbours()
	{
		GridProp* p_gp = GridProp::GetInstance();
		int2 current_tile;
		int2 neighbour_tile;
		int neighbour_counter;
		for (int i = 0; i < ARENA_ROWS; i++)
		{
			for (int j = 0; j < ARENA_COLUMNS; j++)
			{
				neighbour_counter = 0;
				current_tile = int2(i, j);
				/*     __
                    __/  \__
                   /  \__/  \ <-- top row
                   \__/  \__/
                   /  \__/  \ <-- bottom row
                   \__/  \__/
				Check if bottom row or top row	*/
				if (j % 2 != 0)
				{
					neighbour_tile = int2(i + 1, j - 1); //Top left neighbor
					if (CheckIfValidNeighbour(current_tile, neighbour_tile))
					{ 
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
						p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id; // If everything is okey we push it into this id array of neighbours.
					}

					neighbour_counter++;//iterate through the neigbours so they come in the right pos in the array
					neighbour_tile = int2(i + 1, j); //Top neighbor
					if (CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i + 1, j + 1); //Top right neighbor
					if (CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i, j - 1); //Bottom left neighbor
					if (CheckIfValidNeighbour(current_tile, neighbour_tile))
					{ 
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i - 1, j); //Bottom neighbor
					if (CheckIfValidNeighbour(current_tile, neighbour_tile))
					{ 
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i, j + 1); //Bottom right neighbor
					if (CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}
				}
				else
				{
					neighbour_tile = int2(i, j - 1); //Top left neighbor
					if (CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i + 1, j); //Top neighbor
					if (CheckIfValidNeighbour(current_tile, neighbour_tile))
					{ 
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i, j + 1); //Top right neighbor
					if (CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i - 1, j - 1); //Bottom left neighbor
					if (CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i - 1, j); //Bottom neighbor
					if (CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i - 1, j + 1); //Bottom right neighbor
					if (CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}
				}

			}
		}
	}

	int2 FindStartingTile(PLAYER Id)
	{
		/*
		Picture of which corner the players should spawn in
		__________________
		|        |        |
		|        |        |
		|   3    |   4    |
		|________|________|
		|        |        |
		|        |        |
		|   1    |   2    |
		|________|________|
		*/

		//Initialize variables
		int rows = ARENA_ROWS;
		int columns = ARENA_COLUMNS;
		int2 index(-1, -1);
		int min_x, min_y;
		GridProp* p_gp = GridProp::GetInstance();
		//Set the minimum tile index in x- and y-axis depending on which player it is
		switch (Id)
		{
		case PLAYER1:
			min_x = min_y = 0;
			break;
		case PLAYER2:
			min_x = columns / 2;
			min_y = 0;
			break;
		case PLAYER3:
			min_x = 0;
			min_y = rows / 2;
			break;
		case PLAYER4:
			min_x = columns / 2;
			min_y = rows / 2;
			break;
		default:
			break;
		}
		//Initialize the random number generator
		bool tileFound = false;
		int x = 0;
		int y = 0;
		//Randomize an index in the players corner and check if it is a passable tile. If so return that tiles index as the starting tile.
		//The seed is set in AIEcsFunctions.cpp in the CreateArmies function.
		while (!tileFound)
		{
			x = std::rand() % (columns / 2) + min_x;
			y = std::rand() % (rows / 2) + min_y; 
			if (p_gp->mGrid[y][x].isPassable)
			{
				std::cout << "x: " << x << " y: " << y << std::endl; //Used for debug purpose
				index.x = x;
				index.y = y;
				tileFound = true;
			}
		}
		return index;
	}
	int2 GetTileFromWorldPos(float x, float z)
	{
		int2 index;
		index.x = -1;
		index.y = -1;
		GridProp* p_gp = GridProp::GetInstance();
		float pos_x = x;
		float pos_z = z;
		float pi = 3.1415f;
		float mid_to_side = cos(30 * pi / 180) * TILE_RADIUS; //Calculate length between the center position and a side. 
		int steps = 0;

		if (pos_x < TILE_RADIUS)
		{
			index.x = 0;
		}
		else
		{
			while (pos_x > TILE_RADIUS)
			{
				pos_x -= TILE_RADIUS * 2;
				steps++;
			}
			if (steps > ARENA_COLUMNS - 1)
			{
				index.x = ARENA_COLUMNS - 1;
			}
			else
			{
				index.x = steps;
			}
		}
		steps = 0;
		if (pos_z < mid_to_side)
		{
			index.y = 0;
		}
		else
		{
			if (index.x % 2 == 0)
			{
				while (pos_z > mid_to_side)
				{
					pos_z -= mid_to_side * 2;
					steps++;
				}
				if (steps > ARENA_ROWS - 1)
				{
					index.y = ARENA_ROWS - 1;
				}
				else
				{
					index.y = steps;
				}
			}
			else
			{
				while (pos_z > mid_to_side * 2)
				{
					pos_z -= mid_to_side * 2;
					steps++;
				}
				if (steps > ARENA_ROWS - 1)
				{
					index.y = ARENA_ROWS - 1;
				}
				else
				{
					index.y = steps;
				}
			}
		}
		return index;
	}
	
};