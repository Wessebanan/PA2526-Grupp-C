#pragma once

#include "ecs.h"
#include "..//gameAI/AIComponents.h"
#include "GridProp.h"
#include "..//gameUtility/UtilityComponents.h"
#include "..//gameSceneObjects/SceneObjectGlobals.h"

void InitBiomes(ecs::EntityComponentSystem& rECS)
{

	ecs::ComponentIterator itt;
	GridProp* p_gp = GridProp::GetInstance();

	TileData p_start[4] = {
		p_gp->mGrid[0][0],
		p_gp->mGrid[11][0],
		p_gp->mGrid[0][11],
		p_gp->mGrid[11][11],
	};

	for (int i = 0; i < 37; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			ecs::components::TileComponent* p_tile_comp = rECS.getComponentFromEntity<ecs::components::TileComponent>(p_start[j].Id);

			p_tile_comp->biome = (BIOME)j;

			switch (j)
			{
			case 0:
				p_start[j] = p_gp->mGrid[i % 6][i / 6];
				break;
			case 1:
				p_start[j] = p_gp->mGrid[11 - (i % 6)][i / 6];
				break;
			case 2:
				p_start[j] = p_gp->mGrid[i % 6][11 - (i / 6)];
				break;
			case 3:
				p_start[j] = p_gp->mGrid[11 - (i % 6)][11 - (i / 6)];
				break;
			default:
				break;
			}
		}
	}


	itt = rECS.getAllComponentsOfType(ecs::components::TileComponent::typeID);

	for (size_t i = 0; i < 144; i++)
	{
		ecs::components::TileComponent* p_tile_comp = (ecs::components::TileComponent*)itt.next();
		
		ecs::components::TransformComponent* p_tile_tansf_comp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(p_tile_comp->getEntityID());
		ecs::components::ColorComponent* p_tile_color_comp = rECS.getComponentFromEntity<ecs::components::ColorComponent>(p_tile_comp->getEntityID());

		int random = rand() % 50;
		int color_offset = -25 + random;

		switch (p_tile_comp->biome)
		{
		case SWAMP:
			p_tile_tansf_comp->position.y += 0.1f;
			p_tile_tansf_comp->position.y *= 1.2f;
			p_tile_color_comp->color = DirectX::XMFLOAT4(0.0f, 30.0f + color_offset, 30.0f + color_offset, 0.0f);
			break;
		case MOUNTAIN:
			p_tile_tansf_comp->position.y += 0.4f;
			p_tile_tansf_comp->position.y *= 2.2f;
			p_tile_color_comp->color = DirectX::XMFLOAT4(50.0f + color_offset, 50.0f + color_offset, 50.0f + color_offset, 0.0f);
			break;
		case FIELD:
			p_tile_tansf_comp->position.y += 0.2f;
			p_tile_tansf_comp->position.y *= 1.4f;
			p_tile_color_comp->color = DirectX::XMFLOAT4(0.0f, 150.0f + color_offset, 0.0f, 0.0f);
			break;
		case DESERT:
			p_tile_tansf_comp->position.y += 0.1f;
			p_tile_tansf_comp->position.y *= 1.1f;
			p_tile_color_comp->color = DirectX::XMFLOAT4(100.0f + color_offset, 100.0f + color_offset, 00.0f, 0.0f);
			break;
		default:
			break;
		}

	}
	
}