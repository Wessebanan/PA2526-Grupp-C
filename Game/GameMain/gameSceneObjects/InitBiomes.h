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
		ecs::components::TileComponent* tile_comp = (ecs::components::TileComponent*)itt.next();

		tile_comp->biome = (BIOME)(i / 36);
		
		
		ecs::components::ColorComponent* tile_color_comp = rECS.getComponentFromEntity<ecs::components::ColorComponent>(tile_comp->getEntityID());

		switch (tile_comp->biome)
		{
		case SWAMP:
			tile_color_comp->color = DirectX::XMFLOAT4( 0.0f , 30.0f , 30.0f , 0.0f );
			break;
		case MOUNTAIN:
			tile_color_comp->color = DirectX::XMFLOAT4(50.0f, 50.0f, 50.0f, 0.0f);
			break;
		case FIELD:
			tile_color_comp->color = DirectX::XMFLOAT4(0.0f, 200.0f, 130.0f, 0.0f);
			break;
		case DESERT:
			tile_color_comp->color = DirectX::XMFLOAT4(130.0f, 130.0f, 30.0f, 0.0f);
			break;
		default:
			break;
		}

	}
	
}