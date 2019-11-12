#pragma once

#include "ecs.h"
#include "..//gameAI/AIComponents.h"
#include "GridProp.h"
#include "..//gameUtility/UtilityComponents.h"
#include "..//gameSceneObjects/SceneObjectGlobals.h"

void InitBiomes(ecs::EntityComponentSystem& rECS, const int Rows, const int Columns)
{

	ecs::ComponentIterator itt;

	itt = rECS.getAllComponentsOfType(ecs::components::TileComponent::typeID);

	GridProp* p_gp = GridProp::GetInstance();
	int cur_row = 0;
	int cur_col = 0;
	int tile_count = Rows * Columns;
	//Loop over each tile
	for (size_t i = 0; i < tile_count; i++)
	{
		ecs::components::TileComponent* p_tile_comp = (ecs::components::TileComponent*)itt.next();
		cur_row = i / Rows;
		cur_col = i % Columns;
		ecs::components::TransformComponent* p_tile_tansf_comp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(p_tile_comp->getEntityID());
		ecs::components::ColorComponent* p_tile_color_comp = rECS.getComponentFromEntity<ecs::components::ColorComponent>(p_tile_comp->getEntityID());

		int random = rand() % 50;
		int color_offset = -25 + random;

		 //Here the effect each biome has i applied
		switch (p_tile_comp->biome)
		{
		case SNOW:
			p_tile_tansf_comp->position.y += 0.3f;// +(color_offset / 200.0f);
			p_tile_tansf_comp->position.y *= 1.8f;
			p_tile_color_comp->red =	min(182 + color_offset, 255);
			p_tile_color_comp->green =	min(244 + color_offset, 255);
			p_tile_color_comp->blue =	min(248 + color_offset, 255);
			break;
		case MOUNTAIN:
			p_tile_tansf_comp->position.y += 0.3f;// + (color_offset / 200.0f);
			p_tile_tansf_comp->position.y *= 1.9f;
			p_tile_color_comp->red =	min(234 + color_offset, 255);
			p_tile_color_comp->green =  min(110 + color_offset, 255);
			p_tile_color_comp->blue =   min(234 + color_offset, 255);
			break;
		case FIELD:
			p_tile_tansf_comp->position.y += 0.2f;// + (color_offset / 300.0f);
			p_tile_tansf_comp->position.y *= 1.2f;
			p_tile_color_comp->red =	min(105 + color_offset, 255);
			p_tile_color_comp->green =	min(196 + color_offset, 255);
			p_tile_color_comp->blue =	max(16, 0);
			break;
		case DESERT:
			p_tile_tansf_comp->position.y += 0.1f;// + (color_offset / 300.0f);
			p_tile_tansf_comp->position.y *= 1.1f;
			p_tile_color_comp->red = min(248 + color_offset, 255);
			p_tile_color_comp->green = min(236 + color_offset, 255);
			p_tile_color_comp->blue = 179 + color_offset;			
			break;
		default:
			p_tile_tansf_comp->position.y = 1.0f;
			p_tile_color_comp->red = 255;
			p_tile_color_comp->green = 255;
			p_tile_color_comp->blue = 255;
			break;
		}
		if (p_gp->mGrid[cur_row][cur_col].Id == p_tile_tansf_comp->getEntityID())
		{
			p_gp->mGrid[cur_row][cur_col].height = p_tile_tansf_comp->position.y; //put the same height to m_grid so that neighbours for pathfinding get same height as world
		}
		else 
		{
			for (int i = 0; i < Rows; i++)
			{
				for (int j = 0; j < Columns; j++)
				{
					if (p_gp->mGrid[i][j].Id == p_tile_tansf_comp->getEntityID()) // makes sure that the correct id gets same height
					{
						p_gp->mGrid[i][j].height = p_tile_tansf_comp->position.y;
						break;
					}
				}
			}
		}

	}
	//creates neighbours for pathfinding
	GridFunctions::StoreNeighbours();
	GridEcsFunctions::LoadNeighboursToComponents(rECS);

	//Lhure created this but is not in use right now and i dont want to remove it because he might need it in the future
	//for (size_t i = 1; i < Rows-1; i++)		
	//	for (size_t j = 1; j < Columns-1; j++)
	//	{
	//		// not do avarage with water
	//		if (p_gp->mGrid[i][j].height > -0.2f)
	//		{
	//			float avg = 0.0f;
	//			avg += p_gp->mGrid[i][j].height;
	//			avg += p_gp->mGrid[i - 1][j].height;
	//			avg += p_gp->mGrid[i + 1][j].height;
	//			avg += p_gp->mGrid[i][j + 1].height;
	//			avg += p_gp->mGrid[i - 1][j + 1].height;
	//			avg += p_gp->mGrid[i + 1][j + 1].height;
	//			avg += p_gp->mGrid[i][j - 1].height;

	//			avg /= 7;

	//			p_gp->mGrid[i][j].height = avg;
	//		}
	//		
	//	}
	
}