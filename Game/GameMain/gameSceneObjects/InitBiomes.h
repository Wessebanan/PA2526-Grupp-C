#pragma once

#include "ecs.h"
#include "..//gameAI/AIComponents.h"
#include "GridProp.h"
#include "..//gameUtility/UtilityComponents.h"
#include "..//gameSceneObjects/SceneObjectGlobals.h"

void InitBiomes(ecs::EntityComponentSystem& rECS)
{

	ecs::ComponentIterator itt;

	itt = rECS.getAllComponentsOfType(ecs::components::TileComponent::typeID);

	//Loop over each tile
	for (size_t i = 0; i < 144; i++)
	{
		ecs::components::TileComponent* p_tile_comp = (ecs::components::TileComponent*)itt.next();
		
		ecs::components::TransformComponent* p_tile_tansf_comp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(p_tile_comp->getEntityID());
		ecs::components::ColorComponent* p_tile_color_comp = rECS.getComponentFromEntity<ecs::components::ColorComponent>(p_tile_comp->getEntityID());

		int random = rand() % 50;
		int color_offset = -25 + random;

		// Here the effect each biome has i applied
		switch (p_tile_comp->biome)
		{
		case SWAMP:
			p_tile_tansf_comp->position.y += 0.1f;
			p_tile_tansf_comp->position.y *= 1.2f;
			p_tile_color_comp->red = 0;
			p_tile_color_comp->green =  30 + color_offset;
			p_tile_color_comp->blue = 30 + color_offset;
			break;
		case MOUNTAIN:
			p_tile_tansf_comp->position.y += 0.4f;
			p_tile_tansf_comp->position.y *= 2.2f;
			p_tile_color_comp->red = 50 + color_offset;
			p_tile_color_comp->green = 50 + color_offset;
			p_tile_color_comp->blue = 50 + color_offset;
			break;
		case FIELD:
			p_tile_tansf_comp->position.y += 0.2f;
			p_tile_tansf_comp->position.y *= 1.4f;
			p_tile_color_comp->red = 0;
			p_tile_color_comp->green = 150 + color_offset;
			p_tile_color_comp->blue = 0;
			break;
		case DESERT:
			p_tile_tansf_comp->position.y += 0.1f;
			p_tile_tansf_comp->position.y *= 1.1f;
			p_tile_color_comp->red = 100 + color_offset;
			p_tile_color_comp->green = 100 + color_offset;
			p_tile_color_comp->blue = 0;
			break;
		default:
			break;
		}

	}
	
}