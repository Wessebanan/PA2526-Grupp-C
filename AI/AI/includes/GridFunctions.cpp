#include <GridFunctions.h>
#include <UtilityComponents.h>
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
		float heightMap[144];
		CreateHeightmap(heightMap);
		
		//Calculate the position and create every tile.
		for (int i = 0; i < rows; i++)
		{
			//Reset x-pos for every new row and set the starting z-pos for the new row.
			current_pos.x = starting_pos.x;
			current_pos.z = starting_pos.z + i * mid_to_side * 2;
			for (int j = 0; j < columns; j++)
			{
				//test2 = heightMap[(i*12)+j];
				
				//Save the calculated values into the PositionComponent.
				transform.position.x = current_pos.x;
				transform.position.y = heightMap[(i*12)+j];
				transform.position.z = current_pos.z;
				//Create the new entity
				rEcs.createEntity(transform, tile);

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

	}

	void CreateDebugSystems(ecs::EntityComponentSystem& rEcs)
	{
		rEcs.createSystem<systems::TilePrintSystem>();
	}

	void CreateHeightmap(float* arr) //Creates a fixed array that is used to change the hight for the map
		// size is 12x12 this will be changed in the future if creation of dynamic map size is desired 
	{
		float heightValues[144] =
		{ 0.f,0.f,0.f,0.f,1.f,1.f,1.f,0.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,0.f,0.f,1.f,1.f,1.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
		  1.f,1.f,1.f,1.f,1.f,0.f,0.f,1.f,1.f,1.f,1.f,1.f,
		  0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,0.f,2.f,0.f,0.f,
		  0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,0.f,2.f,0.f,0.f,
		  0.f,1.f,2.f,3.f,2.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,1.f,2.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
		  0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f };

		for (int i = 0; i < 144; i++)
		{
			arr[i] = heightValues[i]; 
		}
	}
};