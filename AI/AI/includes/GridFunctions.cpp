#include "GridFunctions.h"
#include "AIGlobals.h"
#include "GridProp.h"
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
		ecs::Entity* current_tile;
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
		
		GridProp* p_gp = GridProp::GetInstance();
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
					p_gp->mGrid[i][j].isPassable = false;
				}
				else if (transform.position.y == 3)
				{
					tile.tileType = STONE;
					tile.impassable = false;
					p_gp->mGrid[i][j].isPassable = true;
				}
				else
				{
					tile.tileType = GRASS;
					tile.impassable = false;
					p_gp->mGrid[i][j].isPassable = true;
				}

				//Create the new entity
				currentTile = rEcs.createEntity(transform, tile);
				//ArenaProperties::tileEntityID[i][j] = currentTile->getID();
				p_gp->mGrid[i][j].Id = currentTile->getID();
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
		CreatePotentialField(rEcs);
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
		  0.f,1.f,2.f,3.f,2.f,1.f,0.f,0.f,5.f,0.f,0.f,0.f,
		  0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };

		for (int i = 0; i < 144; i++)
		{
			arr[i] = height_values[i];
		}
	}

	void CreatePotentialField(ecs::EntityComponentSystem& rEcs)
	{
		//Create a filter to find entities with the given components
		ecs::TypeFilter filter;
		filter.addRequirement(ecs::components::TransformComponent::typeID);
		filter.addRequirement(ecs::components::TileComponent::typeID);

		//An interator that goes through all enities that can be found with the filter
		ecs::EntityIterator entity_iterator = rEcs.getEntititesByFilter(filter);
		float e_x, e_z, o_x, o_z;
		int nr_of_obstacles = 0;
		for (ecs::FilteredEntity entry : entity_iterator.entities)//loops through all tiles 
		{
			nr_of_obstacles = 0;
			if (!entry.getComponent<ecs::components::TileComponent>()->impassable)//temp, checking if itself is -1(impassable)
			{
				e_x = entry.getComponent<ecs::components::TransformComponent>()->position.x; //get pos for tile to measure length to other tiles
				e_z = entry.getComponent<ecs::components::TransformComponent>()->position.z;
				for (ecs::FilteredEntity other : entity_iterator.entities)//loops through all tiles again 
				{
					if (entry.entity->getID() == other.entity->getID()) //Check to see if the tile is itself
					{
						continue;
					}

					if (other.getComponent<ecs::components::TileComponent>()->impassable)//temp because only looking for predefined -1 tiles right now 
					{
						//when obstacle found we see how far away the entry tile is from this tile and give that a charge base on distance
						o_x = other.getComponent<ecs::components::TransformComponent>()->position.x;
						o_z = other.getComponent<ecs::components::TransformComponent>()->position.z;
						entry.getComponent<ecs::components::TileComponent>()->niceness += CreateCharge(e_x, e_z, o_x, o_z,-5);
						nr_of_obstacles++;
					}
					if (other.getComponent<ecs::components::TransformComponent>()->position.y == 5)
					{
						//this is the tile with an attractive charge(goal node)
						o_x = other.getComponent<ecs::components::TransformComponent>()->position.x;
						o_z = other.getComponent<ecs::components::TransformComponent>()->position.z;
						entry.getComponent<ecs::components::TileComponent>()->niceness += CreateCharge(e_x, e_z, o_x, o_z, 50);
					}
				}
				entry.getComponent<ecs::components::TileComponent>()->niceness = entry.getComponent<ecs::components::TileComponent>()->niceness / nr_of_obstacles;
			}
			else
				entry.getComponent<ecs::components::TileComponent>()->niceness = -5.0f;
		}
	}

	float CreateCharge(float startX, float startZ, float endX, float endZ, float charge)
	{
		float to_return = 0.f;
		float x = abs(endX - startX);
		float z = abs(endZ - startZ);
		float dist = sqrt(x * x + z * z);//get the distance from start to end
		dist = dist / ((ArenaProperties::tileRadius) * 4);//scale the distance for better values
		int sign = (int)(fabs(charge) / charge);//get the sign from charge variable "+" or "-"
		to_return = sign*pow(fabs(charge), 1 / (dist + 1));//return a exponentially decreasing value depending on distance

		return to_return;
	}

	int2 FindStartingTile(PLAYER id)
	{
		int rows = ARENA_ROWS;
		int columns = ARENA_COLUMNS;
		int2 index;
		index.x = 0;
		index.y = 0;
		GridProp* p_gp = GridProp::GetInstance();
		bool tile_found = false;
		switch (id)
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
						tile_found = true;
						break;
					}
				}
				if (tile_found)
				{
					break;
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
						tile_found = true;
						break;
					}
				}
				if (tile_found)
				{
					break;
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
						tile_found = true;
						break;
					}
				}
				if (tile_found)
				{
					break;
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
						tile_found = true;
						break;
					}
				}
				if (tile_found)
				{
					break;
				}
			}
			break;
		default:
			break;
		}
		return index;
	}
};