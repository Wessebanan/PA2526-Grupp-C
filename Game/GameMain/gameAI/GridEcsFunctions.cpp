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
					tile.impassable = true;
					tile.goal = false;
					p_gp->mGrid[i][j].isPassable = false;
				}
				else if (transform.position.y == 3)
				{
					tile.tileType = STONE;
					tile.impassable = false;
					tile.goal = false;
					p_gp->mGrid[i][j].isPassable = true;
				}
				else if (transform.position.y == -2)
				{
					tile.tileType = WATER;
					tile.impassable = false;
					p_gp->mGrid[i][j].isPassable = true;
					tile.goal = true;
				}
				else
				{
					tile.tileType = GRASS;
					tile.impassable = false;
					tile.goal = false;
					p_gp->mGrid[i][j].isPassable = true;
				}

				//Create the new entity
				current_tile = rEcs.createEntity(transform, tile);
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
		CreatePotentialField(rEcs);
		GridFunctions::StoreNeighbours();
		LoadNeighboursToComponents(rEcs);
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

	void LoadNeighboursToComponents(ecs::EntityComponentSystem& rEcs)
	{
		GridProp* p_gp = GridProp::GetInstance();
		components::TileComponent* temp_component = nullptr;
		for (int i = 0; i < ARENA_ROWS; i++)
		{
			for (int j = 0; j < ARENA_COLUMNS; j++)
			{
				temp_component = rEcs.getComponentFromEntity<components::TileComponent>(p_gp->mGrid[i][j].Id);
				for (int k = 0; k < 6; k++)
				{
					
					temp_component->neighboursIDArray[k] = p_gp->mGrid[i][j].neighbourIDArray[k];
				}
			}
		}
	}
};