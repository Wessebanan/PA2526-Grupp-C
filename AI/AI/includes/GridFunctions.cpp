#include "GridFunctions.h"
#include "GridProp.h"
#include <DirectXMath.h>
#include <cstdlib>

using namespace DirectX;

namespace GridFunctions
{
	void CreateHeightmap(float* Arr) //Creates a fixed array that is used to change the hight for the map
		// size is 12x12 this will be changed in the future if creation of dynamic map size is desired 
	{
		float height_values[12][12] =
		{ 
			{	-1.f,-1.f,0.f,0.f,1.f,1.f,1.f,0.f,0.f,-1.f,-1.f,-1.f},
			{	-1.f,0.f,0.f,0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,-1.f},
			{	0.f,0.f,0.f,0.f,0.f,1.f,1.f,1.f,0.f,0.f,0.f,0.f},
			{	0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f},
			{	-1.f,-1.f,-1.f,-1.f,-1.f,0.f,0.f,-1.f,-1.f,-1.f,-1.f,-1.f},
			{	0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f},
			{	0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f},
			{	0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,0.f,2.f,0.f,0.f},
			{	0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,0.f,2.f,0.f,0.f},
			{	0.f,1.f,2.f,3.f,2.f,1.f,0.f,0.f,-2.f,0.f,0.f,-1.f},
			{	0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,-1.f},
			{	-1.f,-1.f,0.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,-1.f}
		};

		//int side0 = rand() % 10;
		int side0 = rand() % 9;
		int side1 = rand() % 9;
		int side2 = rand() % 9;
		int side3 = rand() % 9;

		for (size_t i = 0; i < 3; i++)
		{
			height_values[0][(side0 + i)] = -1.0f;
			height_values[11][(side1 + i)] = -1.0f;
			height_values[(side2 + i)][0] = -1.0f;
			height_values[(side3 + i)][11] = -1.0f;
		}

		for (size_t i = 0; i < 2; i++)
		{
			height_values[1][(side0 + i)] = -1.0f;
			height_values[10][(side1 + i)] = -1.0f;
		}

		for (int i = 0; i < 12; i++)
		{
			for (int j = 0; j < 12; j++)
				Arr[j + i*12] = height_values[i][j];
		}
	}

	float CreateCharge(float StartX, float StartZ, float EndX, float EndZ, float Charge)
	{
		float to_return = 0.f;
		float x = fabsf(EndX - StartX);
		float z = fabsf(EndZ - StartZ);
		float dist = sqrt(x * x + z * z);//get the distance from start to end
		dist = dist / ((TILE_RADIUS) * 4);//scale the distance for better values
		int sign = (int)(fabs(Charge) / Charge);//get the sign from charge variable "+" or "-"
		to_return = sign*pow(fabs(Charge), 1 / (dist + 1));//return a exponentially decreasing value depending on distance

		return to_return;
	}

	float GetDistance(float startX, float startZ, float endX, float endZ)
	{
		float to_return = 0.f;
		float x = fabsf(endX - startX);
		float z = fabsf(endZ - startZ);
		float dist = sqrt(x * x + z * z);//get the distance from start to end
		to_return = dist / ((TILE_RADIUS) * 4);//scale the distance for better values
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
		int rows = ARENA_ROWS;
		int columns = ARENA_COLUMNS;
		int2 index;
		index.x = -1;
		index.y = -1;
		GridProp* p_gp = GridProp::GetInstance();
		bool tile_found = false;
		switch (Id)
		{
		case PLAYER1:
			for (int y = 0; y < rows / 2; y++)
			{
				for (int x = 0; x < columns / 2; x++)
				{
					if (p_gp->mGrid[y][x].isPassable)
					{
						index.x = x;
						index.y = y;
						return index;
					}
				}
			}
			break;
		case PLAYER2:
			for (int y = 0; y < rows / 2; y++)
			{
				for (int x = columns - 1; x > columns / 2; x--)
				{
					if (p_gp->mGrid[y][x].isPassable)
					{
						index.x = x;
						index.y = y;
						return index;
					}
				}
			}
			break;
		case PLAYER3:
			for (int y = rows - 1; y > rows / 2; y--)
			{
				for (int x = 0; x < columns / 2; x++)
				{
					if (p_gp->mGrid[y][x].isPassable)
					{
						index.x = x;
						index.y = y;
						return index;
					}
				}
			}
			break;
		case PLAYER4:
			for (int y = rows - 1; y > rows / 2; y--)
			{
				for (int x = columns - 1; x > columns / 2; x--)
				{
					if (p_gp->mGrid[y][x].isPassable)
					{
						index.x = x;
						index.y = y;
						return index;
					}
				}
			}
			break;
		default:
			break;
		}
		return index;
	}
};