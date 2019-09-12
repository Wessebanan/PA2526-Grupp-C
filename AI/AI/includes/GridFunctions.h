#pragma once
#include <ecs.h>
#include "AIComponents.h"
#include "AISystems.h"
#include "AIEvents.h"

using namespace ecs;
using namespace ecs::components;

namespace GridFunctions
{
	//Calculates the centerposition of all the tiles in the grid.
	void InitGrid(ecs::EntityComponentSystem& rEcs, const int rows, const int columns, const float radius)
	{
		float pi = 3.1415f;
		float starting_pos[3] = {0.0f, 0.0f, 0.0f};
		float current_pos[3] = { 0.0f, 0.0f, 0.0f };
		float mid_to_side = cos(30 * pi / 180) * radius; //Calculate length between the center position and a side. 
		PositionComponent position;
		TileComponent tile;

		//Calculate the position and create every tile.
		for (int i = 0; i < rows; i++)
		{
			//Reset x-pos for every new row and set the starting z-pos for the new row.
			current_pos[0] = starting_pos[0];
			current_pos[2] = starting_pos[2] + i * mid_to_side * 2;
			for (int j = 0; j < columns; j++)
			{
				//Save the calculated values into the PositionComponent.
				position.x = current_pos[0];
				position.y = current_pos[1];
				position.z = current_pos[2];
				//Create the new entity
				rEcs.createEntity(position, tile);

				//Update the x-position of the next tile in this row.
				current_pos[0] += 1.5f * radius;
				//Update the z-position of the next tile depending on if it is in a 
				//odd or even column.
				if (j % 2 == 0)
				{
					current_pos[2] += mid_to_side;
				}
				else
				{
					current_pos[2] -= mid_to_side;
				}
			}
		}
	
	}

	void CreateDebugSystems(ecs::EntityComponentSystem& rEcs)
	{
		rEcs.createSystem<systems::TilePrintSystem>();
	}
};