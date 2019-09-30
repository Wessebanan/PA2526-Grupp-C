#include "GridFunctions.h"
#include "AIGlobals.h"
#include "UtilityComponents.h"
#include <DirectXMath.h>

using namespace ecs;
using namespace ecs::components;
using namespace DirectX;

namespace GridFunctions
{
	//Calculates the centerposition of all the tiles in the grid.
	void CreateGrid(ecs::EntityComponentSystem& rEcs, const int rows, const int columns, const float radius)
	{
		float pi = 3.1415f;
		XMFLOAT3 starting_pos = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 current_pos = { 0.0f, 0.0f, 0.0f };
		float mid_to_side = cos(30 * pi / 180) * radius; //Calculate length between the center position and a side. 
		TransformComponent transform;
		TileComponent tile;
		ecs::Entity* currentTile;
		float height_map[144];
		CreateHeightmap(height_map);
		//for (int i = 0; i < rows; i++)
		//{
		//	ArenaProperties::gridLogic[0][i].entityID = i;
		//}
		//for (int i = 0; i < rows; i++)
		//{
		//	std::cout << ArenaProperties::gridLogic[0][i].entityID << std::endl;
		//}
		
		//Calculate the position and create every tile.
		for (int i = 0; i < rows; i++)
		{
			//Reset x-pos for every new row and set the starting z-pos for the new row.
			current_pos.x = starting_pos.x;
			current_pos.z = starting_pos.z + i * mid_to_side * 2;
			for (int j = 0; j < columns; j++)
			{
				
				//Save the calculated values into the PositionComponent.
				transform.position.x = current_pos.x;
				transform.position.y = height_map[(i*12)+j];
				transform.position.z = current_pos.z;
				if (transform.position.y == -1.f)
				{
					tile.tileType = WATER;
					tile.impassable = true;
					ArenaProperties::gridPassable[i][j] = false;
				}
				else if (transform.position.y == 3)
				{
					tile.tileType = STONE;
					tile.impassable = false;
					ArenaProperties::gridPassable[i][j] = true;
				}
				else
				{
					tile.tileType = GRASS;
					tile.impassable = false;
					ArenaProperties::gridPassable[i][j] = ArenaProperties::gridID[i][j] = true;
				}

				//Create the new entity
				currentTile = rEcs.createEntity(transform, tile);
				ArenaProperties::test = 10;
				ArenaProperties::MackeID[i][j] = 1337; // = currentTile->getID();
				ArenaProperties::test = currentTile->getID();
				int testing = ArenaProperties::gridID[i][j];
				std::cout << ArenaProperties::gridID[i][j] << std::endl;
				ArenaProperties::gridPassable;
				ArenaProperties::gridID;
				ArenaProperties::MackeID;
				//Update the x-position of the next tile in this row.
				current_pos.x += 1.5f * radius;
				//Update the z-position of the next tile depending on if it is in a 
				//odd or even column.
				if (j % 2 == 0)
				{
					current_pos.z += mid_to_side;
				}
				else
				{
					current_pos.z -= mid_to_side;
				}
			}
		}
		
		int dank = 0;
	}

	void CreateDebugSystems(ecs::EntityComponentSystem& rEcs)
	{
		rEcs.createSystem<systems::TilePrintSystem>();
	}

	void CreateHeightmap(float* arr) //Creates a fixed array that is used to change the hight for the map
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
			arr[i] = height_values[i];
		}
	}

	int2 FindStartingTile(PLAYER id)
	{
		int2 index;
		index.x = 0;
		index.y = 0;
		/*switch (id)
		{
		case PLAYER1:
			for (int y = 0; y < ArenaProperties::rows / 2; y++)
			{
				for (int x = 0; x < ArenaProperties::columns / 2; x++)
				{
					if (ArenaProperties::gridLogic[y][x].isPassable)
					{
						index.x = x;
						index.y = y;
						break;
					}
				}
			}
			break;
		case PLAYER2:
			for (int y = 0; y < ArenaProperties::rows / 2; y++)
			{
				for (int x = ArenaProperties::columns - 1; x > ArenaProperties::columns / 2; x--)
				{
					if (ArenaProperties::gridLogic[y][x].isPassable)
					{
						index.x = x;
						index.y = y;
						break;
					}
				}
			}
			break;
		case PLAYER3:
			for (int y = ArenaProperties::rows - 1; y > ArenaProperties::rows / 2; y--)
			{
				for (int x = 0; x < ArenaProperties::columns / 2; x++)
				{
					if (ArenaProperties::gridLogic[y][x].isPassable)
					{
						index.x = x;
						index.y = y;
						break;
					}
				}
			}
			break;
		case PLAYER4:
			for (int y = ArenaProperties::rows - 1; y > ArenaProperties::rows / 2; y--)
			{
				for (int x = ArenaProperties::columns - 1; x > ArenaProperties::columns / 2; x--)
				{
					if (ArenaProperties::gridLogic[y][x].isPassable)
					{
						index.x = x;
						index.y = y;
						break;
					}
				}
			}
			break;
		default:
			break;
		}*/
		return index;
	}
};