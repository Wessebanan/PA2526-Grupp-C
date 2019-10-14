#include "GridEcsFunctions.h"
#include "AIGlobals.h"
#include "GridProp.h"

#include "../gameUtility/UtilityComponents.h"
#include "../../AI/includes/GridFunctions.h"
#include <DirectXMath.h>

using namespace ecs;
using namespace ecs::components;
using namespace DirectX;

namespace GridEcsFunctions
{
	//Calculates the centerposition of all the tiles in the grid.
	void CreateGrid(ecs::EntityComponentSystem& rEcs, const int Rows, const int Columns, const float Radius)
	{
		float pi = 3.1415f;
		XMFLOAT3 starting_pos = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 current_pos = { 0.0f, 0.0f, 0.0f };
		float mid_to_side = cos(30 * pi / 180) * Radius; //Calculate length between the center position and a side. 
		TransformComponent transform;
		ColorComponent color;
		TileComponent tile;
		ecs::Entity* current_tile;
		float height_map[ARENA_COLUMNS*ARENA_ROWS];
		GridFunctions::CreateHeightmap(height_map);
		//for (int i = 0; i < rows; i++)
		//{
		//	ArenaProperties::gridLogic[0][i].entityID = i;
		//}
		//for (int i = 0; i < rows; i++)
		//{
		//	std::cout << ArenaProperties::gridLogic[0][i].entityID << std::endl;
		//}
		
		GridProp* p_gp = GridProp::GetInstance();

		color.red = 0;
		color.green = 0;
		color.blue = 0;

		//Calculate the position and create every tile.
		for (int i = 0; i < Rows; i++)
		{
			//Reset x-pos for every new row and set the starting z-pos for the new row.
			current_pos.x = starting_pos.x;
			current_pos.z = starting_pos.z + i * mid_to_side * 2;
			for (int j = 0; j < Columns; j++)
			{
				
				//Save the calculated values into the PositionComponent.
				transform.position.x = current_pos.x;
				transform.position.y = height_map[(i*12)+j];
				transform.position.z = current_pos.z;
				if (transform.position.y == -1.f)
				{
					tile.tileType = WATER;
					color.blue = 150.0f;
					tile.impassable = true;
					tile.goal = false;
					p_gp->mGrid[i][j].isPassable = false;
					p_gp->mGrid[i][j].biome = -1;
				}
				else if (transform.position.y == 3)
				{
					tile.tileType = GAME_FIELD;
					tile.impassable = false;
					tile.goal = false;
					p_gp->mGrid[i][j].isPassable = true;
					p_gp->mGrid[i][j].biome = -1;
				}
				else if (transform.position.y == -2)
				{
					tile.tileType = WATER;
					color.blue = 150.0f;
					tile.impassable = false;
					p_gp->mGrid[i][j].isPassable = true;
					p_gp->mGrid[i][j].biome = -1;
					tile.goal = true;
				}
				else
				{
					tile.tileType = GAME_FIELD;
					tile.impassable = false;
					tile.goal = false;
					p_gp->mGrid[i][j].isPassable = true;
					p_gp->mGrid[i][j].biome = -1;
				}

				//Create the new entity
				current_tile = rEcs.createEntity(transform, color, tile);
				p_gp->mGrid[i][j].Id = current_tile->getID();
				p_gp->mGrid[i][j].height = transform.position.y;
				//Update the x-position of the next tile in this row.
				current_pos.x += 1.5f * Radius;
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

		for (int i = 0; i < Rows; i++)
		{
			for (int j = 0; j < Columns; j++)
			{
				int random_biome = rand() % 4;

				//int x_cord = random_biome % 2;
				//int y_cord = 0;
				//if (random_biome > 1)
				//	y_cord = 1;
				//x_cord *= 11;
				//y_cord *= 11;

				bool found = false;


				if (random_biome == 0)
				{
					for (int it = 0; it < Rows && !found; it++)
					{
						for (int jt = 0; jt < Columns && !found; jt++)
						{
							if (p_gp->mGrid[it][jt].biome == -1)
							{
								p_gp->mGrid[it][jt].biome = random_biome;

								found = true;
							}
						}
					}
				}
				else if (random_biome == 1)
				{
					for (int it = 11; it > 0 && !found; it--)
					{
						for (int jt = 0; jt < Columns && !found; jt++)
						{
							if (p_gp->mGrid[it][jt].biome == -1)
							{
								p_gp->mGrid[it][jt].biome = random_biome;

								found = true;
							}
						}
					}

				}
				else if (random_biome == 2)
				{
					for (int it = 0; it < Rows && !found; it++)
					{
						for (int jt = 11; jt > 0 && !found; jt--)
						{
							if (p_gp->mGrid[it][jt].biome == -1)
							{
								p_gp->mGrid[it][jt].biome = random_biome;

								found = true;
							}
						}
					}

				}
				else if (random_biome == 3)
				{
					for (int it = 11; it > 0 && !found; it--)
					{
						for (int jt = 11; jt > 0 && !found; jt--)
						{
							if (p_gp->mGrid[it][jt].biome == -1)
							{
								p_gp->mGrid[it][jt].biome = random_biome;

								found = true;
							}
						}
					}

				}
			}
		}
		for (int i = 0; i < Rows; i++)
		{
			for (int j = 0; j < Columns; j++)
			{

				ecs::components::TileComponent* p_tile_comp = rEcs.getComponentFromEntity<ecs::components::TileComponent>(p_gp->mGrid[i][j].Id);

				p_tile_comp->biome = (BIOME)p_gp->mGrid[i][j].biome;

			}
		}

		CreatePotentialField(rEcs);
		GridFunctions::StoreNeighbours();
	}

	void CreateDebugSystems(ecs::EntityComponentSystem& rEcs)
	{
		rEcs.createSystem<systems::TilePrintSystem>();
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
		float dist = 0;
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
						entry.getComponent<ecs::components::TileComponent>()->niceness += GridFunctions::CreateCharge(e_x, e_z, o_x, o_z, 10);
						nr_of_obstacles++;
					}
					if (other.getComponent<ecs::components::TransformComponent>()->position.y == -2.f)
					{
						//this is the tile with an attractive charge(goal node)
						o_x = other.getComponent<ecs::components::TransformComponent>()->position.x;
						o_z = other.getComponent<ecs::components::TransformComponent>()->position.z;
						dist = GridFunctions::GetDistance(e_x, e_z, o_x, o_z);
					}
				}
				entry.getComponent<ecs::components::TileComponent>()->niceness = entry.getComponent<ecs::components::TileComponent>()->niceness / nr_of_obstacles;
				entry.getComponent<ecs::components::TileComponent>()->niceness += dist;
			}
			else
				entry.getComponent<ecs::components::TileComponent>()->niceness = 10.0f;
			if (entry.getComponent<ecs::components::TransformComponent>()->position.y == -2.f)
				entry.getComponent<ecs::components::TileComponent>()->niceness = -10.f;
		}
	}

	std::vector<unsigned int> FindPath(ecs::EntityComponentSystem& rEcs, unsigned int startID, unsigned int endID)
	{
		std::vector<unsigned int> to_return;
		components::TileComponent* current_tile = nullptr;
		unsigned int next_tile_id = 0;
		unsigned int current_tile_id = startID;
		unsigned int last_tile_id = 0;
		float niceTry;
		current_tile = rEcs.getComponentFromEntity<components::TileComponent>(startID);
		while (current_tile_id != endID)
		{
			niceTry = 999.f;
			for (int i = 0; i < 6; i++)
			{	//check if neighbour is not 0 or was the last visited tile
				if(current_tile->neighboursIDArray[i] != 0 && current_tile->neighboursIDArray[i] != last_tile_id)
				{	//check for the lowest niceness then that is the next tile
					if(rEcs.getComponentFromEntity<components::TileComponent>(current_tile->neighboursIDArray[i])->niceness < niceTry)
					{
						niceTry = rEcs.getComponentFromEntity<components::TileComponent>(current_tile->neighboursIDArray[i])->niceness;
						next_tile_id = current_tile->neighboursIDArray[i];
					}
				}
			}
			if (next_tile_id != 0)
			{
				current_tile = rEcs.getComponentFromEntity<components::TileComponent>(next_tile_id);
				to_return.push_back(next_tile_id);
				last_tile_id = current_tile_id;
				current_tile_id = next_tile_id;
			}
		}
		return to_return;
	}

	void StoreNeighbours(ecs::EntityComponentSystem& rEcs)
	{
		GridProp* p_gp = GridProp::GetInstance();
		int2 current_tile;
		int2 neighbour_tile;
		int neighbour_counter;
		components::TileComponent* p_tile_component = nullptr;
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
					if (GridFunctions::CheckIfValidNeighbour(current_tile, neighbour_tile))
					{ 
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
						p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id; // If everything is okey we push it into this id array of neighbours.
						//find tilecomponent from entity with id from GridProp singelton 
						p_tile_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[current_tile.x][current_tile.y].Id);
						//Update tile components neighbours
						p_tile_component->neighboursIDArray[neighbour_counter] = p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;//iterate through the neigbours so they come in the right pos in the array
					neighbour_tile = int2(i + 1, j); //Top neighbor
					if (GridFunctions::CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
						p_tile_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[current_tile.x][current_tile.y].Id);
						p_tile_component->neighboursIDArray[neighbour_counter] = p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i + 1, j + 1); //Top right neighbor
					if (GridFunctions::CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
						p_tile_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[current_tile.x][current_tile.y].Id);
						p_tile_component->neighboursIDArray[neighbour_counter] = p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i, j - 1); //Bottom left neighbor
					if (GridFunctions::CheckIfValidNeighbour(current_tile, neighbour_tile))
					{ 
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
						p_tile_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[current_tile.x][current_tile.y].Id);
						p_tile_component->neighboursIDArray[neighbour_counter] = p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i - 1, j); //Bottom neighbor
					if (GridFunctions::CheckIfValidNeighbour(current_tile, neighbour_tile))
					{ 
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
						p_tile_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[current_tile.x][current_tile.y].Id);
						p_tile_component->neighboursIDArray[neighbour_counter] = p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i, j + 1); //Bottom right neighbor
					if (GridFunctions::CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
						p_tile_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[current_tile.x][current_tile.y].Id);
						p_tile_component->neighboursIDArray[neighbour_counter] = p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}
				}
				else
				{
					neighbour_tile = int2(i, j - 1); //Top left neighbor
					if (GridFunctions::CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
						p_tile_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[current_tile.x][current_tile.y].Id);
						p_tile_component->neighboursIDArray[neighbour_counter] = p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i + 1, j); //Top neighbor
					if (GridFunctions::CheckIfValidNeighbour(current_tile, neighbour_tile))
					{ 
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
						p_tile_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[current_tile.x][current_tile.y].Id);
						p_tile_component->neighboursIDArray[neighbour_counter] = p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i, j + 1); //Top right neighbor
					if (GridFunctions::CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
						p_tile_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[current_tile.x][current_tile.y].Id);
						p_tile_component->neighboursIDArray[neighbour_counter] = p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i - 1, j - 1); //Bottom left neighbor
					if (GridFunctions::CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
						p_tile_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[current_tile.x][current_tile.y].Id);
						p_tile_component->neighboursIDArray[neighbour_counter] = p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i - 1, j); //Bottom neighbor
					if (GridFunctions::CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
						p_tile_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[current_tile.x][current_tile.y].Id);
						p_tile_component->neighboursIDArray[neighbour_counter] = p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}

					neighbour_counter++;
					neighbour_tile = int2(i - 1, j + 1); //Bottom right neighbor
					if (GridFunctions::CheckIfValidNeighbour(current_tile, neighbour_tile))
					{
						p_gp->mGrid[current_tile.x][current_tile.y].neighbourIDArray[neighbour_counter] =
							p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
						p_tile_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[current_tile.x][current_tile.y].Id);
						p_tile_component->neighboursIDArray[neighbour_counter] = p_gp->mGrid[neighbour_tile.x][neighbour_tile.y].Id;
					}
				}

			}
		}
	}
};