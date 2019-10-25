#include "GridFunctions.h"
#include "GridProp.h"
#include <DirectXMath.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace DirectX;

namespace GridFunctions
{
	void CreateHeightmap(float* Arr, int rows, int columns, float height_power, int mountains) //Creates a array that is used to change the hight for the map and remove chunks for water
		// size is 12x12 this will be changed in the future if creation of dynamic map size is desired 
	{
		int layers = 3;
		int holme_space = 0;

		// The maximum amount of allowed tiles
		const int max_rows = MAX_ARENA_ROWS; // Adds 10 in the end to allow a holme
		const int max_columns = MAX_ARENA_COLUMNS;


		float height_values[max_rows][max_columns];

		// evrything outside of hte map should be water
		for (size_t i = 0; i < max_rows; i++)
			for (size_t j = 0; j < max_columns; j++)
			{
				height_values[i][j] = -1.f;
			}

		// Default the map to be flat
		for (size_t i = holme_space; i < rows - holme_space; i++)
			for (size_t j = holme_space; j < columns - holme_space; j++)
			{
				height_values[i][j] = 0.f;
			}


		// ------------------ ADD MOUNTINS TO THE MAP

		for (size_t mountain = 0; mountain < mountains; mountain++)
		{
			int top_x = rand() % (rows - 3 - holme_space);
			int top_y = rand() % (columns - 3 - holme_space);
			top_x += 2 + holme_space;
			top_y += 2 + holme_space;
			float top_height = 1.2f * height_power;
			float slope = 0.7f;
			
			// top
			height_values[top_y][top_x] = top_height;

			
			// first circle
			top_height *= slope;

			height_values[top_y][top_x - 1] = top_height;
			height_values[top_y][top_x + 1] = top_height;

			height_values	[top_y - 1][top_x] = top_height;
			height_values[top_y - 1][top_x + 1] = top_height;
			height_values[top_y - 1][top_x - 1] = top_height;

			height_values[top_y + 1][top_x] = top_height;

			//------

			// second circle
			top_height *= slope;

			height_values[top_y][top_x - 2] = top_height;
			height_values[top_y][top_x + 2] = top_height;
			height_values[top_y - 1][top_x - 2] = top_height;
			height_values[top_y - 1][top_x + 2] = top_height;
			height_values[top_y + 1][top_x - 2] = top_height;
			height_values[top_y + 1][top_x + 2] = top_height;

			height_values[top_y + 1][top_x - 1] = top_height;
			height_values[top_y + 1][top_x + 1] = top_height;

			height_values[top_y + 2][top_x] = top_height;

			height_values[top_y - 2][top_x] = top_height;
			height_values[top_y - 2][top_x - 1] = top_height;
			height_values[top_y - 2][top_x + 1] = top_height;

			//------

			// third circle
			top_height *= slope;

			height_values[top_y][top_x - 3] = top_height;
			height_values[top_y][top_x + 3] = top_height;

			height_values[top_y - 1][top_x - 3] = top_height;
			height_values[top_y - 1][top_x + 3] = top_height;
			height_values[top_y + 1][top_x - 3] = top_height;
			height_values[top_y + 1][top_x + 3] = top_height;
			height_values[top_y - 2][top_x - 3] = top_height;
			height_values[top_y - 2][top_x + 3] = top_height;

			height_values[top_y + 2][top_x - 1] = top_height;
			height_values[top_y + 2][top_x - 2] = top_height;
			height_values[top_y + 2][top_x + 1] = top_height;
			height_values[top_y + 2][top_x + 2] = top_height;

			height_values[top_y + 3][top_x] = top_height;

			height_values[top_y - 2][top_x - 2] = top_height;
			height_values[top_y - 2][top_x + 2] = top_height;

			height_values[top_y - 3][top_x] = top_height;
			height_values[top_y - 3][top_x - 1] = top_height;
			height_values[top_y - 3][top_x + 1] = top_height;
			//------
		}


		// ------------ TAKE OUT PARTS OF THE MAP
		holme_space = layers * 2;
		// Removes chunks from each side of the map
		int chunk_size = rows / 4;
		int side0 = rand() % (rows - chunk_size - holme_space);
		int side1 = rand() % (columns - chunk_size - holme_space);
		int side2 = rand() % (rows - chunk_size - holme_space);
		int side3 = rand() % (columns - chunk_size - holme_space);

		side0 += layers;
		side1 += layers;
		side2 += layers;
		side3 += layers;

		// removed 3 on each side
		for (size_t i = 0; i < chunk_size; i++)
		{
			height_values[layers][(side0 + i)] = -1.0f;
			height_values[rows - 1 - layers][(side1 + i)] = -1.0f;
			height_values[(side2 + i)][layers] = -1.0f;
			height_values[(side3 + i)][columns - 1 - layers] = -1.0f;
		}

		// removes 2 more from 2 sides one layer close to the center
		for (size_t i = 0; i < chunk_size-1; i++)
		{
			height_values[1 + layers][(side0 + i)] = -1.0f;
			height_values[rows - 2 - layers][(side1 + i)] = -1.0f;
		}
		

		// If the map is big enough more is removed
		if (rows > 16 + layers && columns > 16 + layers)
		{
			for (size_t j = 0; j < 2; j++)
			{
				// lake things
				//int random_lakeside = 3;
				int random_lakeside = rand() % 4;

				int depth = columns / 6;
				int starting_width = rows / 3;
				int starting_tile = 4;// (rand() % rows) - (starting_width);

				switch (random_lakeside)
				{
				case 0:
				case 2:
					depth = columns / 6;
					starting_width = rows / 3;
					starting_tile = (rand() % rows) - (starting_width);
					break;
				case 1:
				case 3:
					depth = rows / 6;
					starting_width = columns / 3;
					starting_tile = (rand() % columns) - (starting_width);
					break;
				default:
					break;
				}


				// How Deep
				for (size_t k = 0; k < depth; k++)
				{
					// next layer
					for (size_t i = 0; i < starting_width - (2 * k); i++)
					{
						int x = 0;
						int y = 0;

						switch (random_lakeside)
						{
						case 0:
							x = (starting_tile) + i + k;
							y = k + layers;
							break;
						case 1:
							x = k + layers;
							y = (starting_tile) + i + k;
							break;
						case 2:
							x = (starting_tile) + i + k;
							y = columns - k - 1 - layers;
							break;
						case 3:
							x = rows - 1 - k - layers;
							y = (starting_tile) + i + k;
							break;
						default:
							break;
						}

						x = abs(x);
						y = abs(y);
						height_values[x][y] = -1.f;
					}
				}

				// How deep
				for (size_t k = 0; k < depth; k++)
				{
					// How wide
					for (size_t i = 0; i < starting_width - (depth) - 1; i++)
					{
						int x = 0;
						int y = 0;

						switch (random_lakeside)
						{
						case 0:
							x = (starting_tile) + i + (depth / 2);
							y = depth + k + layers;
							break;
						case 1:
							x = depth + k + layers;
							y = (starting_tile) + i + (depth / 2);
							break;
						case 2:
							x = (starting_tile) + i + (depth / 2);
							y = columns - (depth + k) - 1 - layers;
							break;
						case 3:
							x = rows - 1 - (depth + k) - layers;
							y = (starting_tile) + i + (depth / 2);
							break;
						default:
							break;
						}

						if (x < 0)
							x = 0;
						if (y < 0)
							y = 0;
						height_values[x][y] = -1.f;
					}
				}
			}


			// More chunks
			side0 = rand() % (rows - chunk_size);
			side1 = rand() % (columns - chunk_size);
			side2 = rand() % (rows - chunk_size);
			side3 = rand() % (columns - chunk_size);

			// removed 3 on each side
			for (size_t i = 0; i < chunk_size; i++)
			{
				height_values[0][(side0 + i)] = -1.0f;
				height_values[rows - 1][(side1 + i)] = -1.0f;
				height_values[(side2 + i)][0] = -1.0f;
				height_values[(side3 + i)][columns - 1] = -1.0f;
			}

		}

		// -------------- REMOVE 3 LAYERS ON EACH SIDE
		// removed layers on each side
		for (size_t l = 0; l < layers; l++)
		{
			for (size_t i = 0; i < rows; i++)
			{
				height_values[l][(i)] = -1.0f;
				height_values[rows - 1 - l][(i)] = -1.0f;
			}

			for (size_t i = 0; i < columns; i++)
			{
				height_values[(i)][l] = -1.0f;
				height_values[(i)][columns - 1 - l] = -1.0f;
			}
		}

		// Add the holmes
		int start0 = rows / 2;
		int start1 = columns / 2;
		int start2 = start1;
		int start3 = start2;


		size_t i = 0;
		int start = 0;

		// first side
		start = start0;
		height_values[start + 1][0] = 0.0f;
		height_values[start - 1][0] = 0.0f;
		height_values[start - 1][1] = 0.0f;
		for (i = 0; height_values[start][i] < -0.5f; i++)
		{
			height_values[start][i] = 0.0f;
			height_values[start + 1][i] = 0.0f;
		}


		// second side
		start = start0;
		height_values[0][start + 1] = 0.0f;
		height_values[0][start - 1] = 0.0f;
		height_values[1][start - 1] = 0.0f;
		for (i = 0; height_values[i][start] < -0.5f; i++)
		{
			height_values[i][start] = 0.0f;
			height_values[i][start + 1] = 0.0f;
		}

		// third side
		start = start1;
		height_values[start + 1][columns - 1] = 0.0f;
		height_values[start - 1][columns - 1] = 0.0f;
		height_values[start - 1][columns - 2] = 0.0f;
		for (i = 0; height_values[start][columns - 1 - i] < -0.5f; i++)
		{
			height_values[start][columns - 1 - i] = 0.0f;
			height_values[start + 1][columns - 1 - i] = 0.0f;
		}

		// forth side
		start = start1;
		height_values[rows - 1][start + 1] = 0.0f;
		height_values[rows - 1][start - 1] = 0.0f;
		height_values[rows - 2][start - 1] = 0.0f;
		for (i = 0; height_values[rows - 1 - i][start] < -0.5f; i++)
		{
			height_values[rows - 1 - i][start] = 0.0f;
			height_values[rows - 1 - i][start + 1] = 0.0f;
		}



		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
				Arr[(j) + (i) * MAX_ARENA_ROWS] = height_values[i][j];
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
		to_return = sign*pow(fabs(Charge), 1.2 / (dist + 1.f));//return a exponentially decreasing value depending on distance

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
		if (neighbourIndex.x >= 0 && neighbourIndex.x < p_gp->GetSize().x
			&& neighbourIndex.y >= 0 && neighbourIndex.y < p_gp->GetSize().y
			&& p_gp->mGrid[currentTile.x][currentTile.y].height -
			p_gp->mGrid[neighbourIndex.x][neighbourIndex.y].height >= -1.0f
			&& p_gp->mGrid[neighbourIndex.x][neighbourIndex.y].isPassable)
			returnValue = true;

		return returnValue;
	}

	void StoreNeighbours()
	{
		GridProp* p_gp = GridProp::GetInstance();
		int2 current_tile;
		int2 neighbour_tile;
		int neighbour_counter;
		for (int i = 0; i < p_gp->GetSize().x; i++)
		{
			for (int j = 0; j < p_gp->GetSize().y; j++)
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

	// Setting will set it as one of the ways to start
	int2 FindStartingTile(PLAYER Id, int Rows, int Columns, MAPINITSETTING Setting)
	{
		GridProp* p_gp = GridProp::GetInstance();
		int2 index(-1, -1);
		// finds each holme and sets them to it
		if (Setting == MAPINITSETTING::HOLMES)
		{
			switch (Id)
			{
			case PLAYER1:
				for (size_t i = 0; i < Rows; i++)
				{
					if (p_gp->mGrid[i][0].isPassable)
					{
						index.x = i;
						index.y = 0;
						break;
					}
				}
				break;
			case PLAYER2:
				for (size_t i = 0; i < Columns; i++)
				{
					if (p_gp->mGrid[0][i].isPassable)
					{
						index.x = 0;
						index.y = i;
						break;
					}
				}
				break;
			case PLAYER3:
				for (size_t i = 0; i < Rows; i++)
				{
					if (p_gp->mGrid[i][Columns - 1].isPassable)
					{
						index.x = i;
						index.y = Columns - 1;
						break;
					}
				}
				break;
			case PLAYER4:
				for (size_t i = 0; i < Columns; i++)
				{
					if (p_gp->mGrid[Rows - 1][i].isPassable)
					{
						index.x = Rows - 1;
						index.y = i;
						break;
					}
				}
				break;
			default:
				break;
			}
		}
		else // MAPINITSETTING::BIOMES
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
			int rows = Rows;
			int columns = Columns;
			int min_x, min_y;
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
		}
		
		return index;
	}
	int2 GetTileFromWorldPos(float x, float z)
	{
		int2 index;
		index.x = -1;
		index.y = -1;
		GridProp* p_gp = GridProp::GetInstance();
		float pos_x = 0;
		float pos_z = 0;
		float pi = 3.1415f;
		float mid_to_side = cos(30 * pi / 180) * TILE_RADIUS; //Calculate length between the center position and a side. 
		int steps = 0;

		if (x - pos_x < TILE_RADIUS)
		{
			index.x = 0;
		}
		else
		{
			while (x - pos_x > TILE_RADIUS)
			{
				pos_x += TILE_RADIUS * 1.5f;
				steps++;
			}
			if (steps > p_gp->GetSize().y - 1)
			{
				index.x = p_gp->GetSize().y - 1;
			}
			else
			{
				index.x = steps;
			}
		}
		steps = 0;
		if (index.x % 2 != 0)
		{
			pos_z += mid_to_side;
		}
		if (z - pos_z < mid_to_side)
		{
			index.y = 0;
		}
		else
		{
			while (z - pos_z > mid_to_side)
			{
				pos_z += mid_to_side * 2;
				steps++;
			}
			if (steps > MAX_ARENA_ROWS - 1)
			{
				index.y = MAX_ARENA_ROWS - 1;
			}
			else
			{
				index.y = steps;
			}
		}
		return index;
	}
	
};