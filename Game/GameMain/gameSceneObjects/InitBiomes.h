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


	int tile_count = Rows * Columns;
	//Loop over each tile
	for (size_t i = 0; i < tile_count; i++)
	{
		ecs::components::TileComponent* p_tile_comp = (ecs::components::TileComponent*)itt.next();
		
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
			break;
		}


	}
	

	GridProp* p_gp = GridProp::GetInstance();

	for (size_t i = 1; i < Rows-1; i++)
		for (size_t j = 1; j < Columns-1; j++)
		{
			// not do avarage with water
			if (p_gp->mGrid[i][j].height > -0.2f)
			{
				float avg = 0.0f;
				avg += p_gp->mGrid[i][j].height;
				avg += p_gp->mGrid[i - 1][j].height;
				avg += p_gp->mGrid[i + 1][j].height;
				avg += p_gp->mGrid[i][j + 1].height;
				avg += p_gp->mGrid[i - 1][j + 1].height;
				avg += p_gp->mGrid[i + 1][j + 1].height;
				avg += p_gp->mGrid[i][j - 1].height;

				avg /= 7;

				p_gp->mGrid[i][j].height = avg;
			}
			
		}

}