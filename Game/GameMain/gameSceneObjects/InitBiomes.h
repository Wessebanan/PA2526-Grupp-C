#pragma once

#include "ecs.h"
#include "..//gameAI/AIComponents.h"
#include "..//gameUtility/UtilityComponents.h"
#include "..//gameSceneObjects/SceneObjectGlobals.h"


void InitBiomes(ecs::EntityComponentSystem& rECS)
{

	ecs::ComponentIterator itt;
	itt = rECS.getAllComponentsOfType(ecs::components::TileComponent::typeID);

	for (size_t i = 0; i < 144; i++)
	{
		ecs::components::TileComponent* p_tile_comp = (ecs::components::TileComponent*)itt.next();

		p_tile_comp->biome = (BIOME)(i / 36);
		
		
		ecs::components::ColorComponent* p_tile_color_comp = rECS.getComponentFromEntity<ecs::components::ColorComponent>(p_tile_comp->getEntityID());

		int random = rand() % 50;
		int color_offset = -25 + random;

		switch (p_tile_comp->biome)
		{
		case SWAMP:
			p_tile_color_comp->color = DirectX::XMFLOAT4( 0.0f , 30.0f + color_offset , 30.0f + color_offset, 0.0f );
			break;
		case MOUNTAIN:
			p_tile_color_comp->color = DirectX::XMFLOAT4(50.0f + color_offset, 50.0f + color_offset, 50.0f + color_offset, 0.0f);
			break;
		case FIELD:
			p_tile_color_comp->color = DirectX::XMFLOAT4(0.0f, 150.0f + color_offset, 0.0f, 0.0f);
			break;
		case DESERT:
			p_tile_color_comp->color = DirectX::XMFLOAT4(100.0f + color_offset, 100.0f + color_offset, 00.0f, 0.0f);
			break;
		default:
			break;
		}

	}
	
}