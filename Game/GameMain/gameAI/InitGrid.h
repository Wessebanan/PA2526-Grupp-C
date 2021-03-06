#pragma once

#include "ecs.h"

#include "../../AI/AI/includes/GridProp.h"
#include "..//gameAI/GridEcsFunctions.h"
#include "..//gameAI/AIComponents.h"
#include "..//gameUtility/UtilityComponents.h"
#include "../gameSceneObjects/InitBiomes.h"
#include <DirectXMath.h>

void CreateCollisionForTiles(ecs::EntityComponentSystem& rECS);

void InitGrid(ecs::EntityComponentSystem& rECS)
{
	GridProp* p_gp = GridProp::GetInstance();
	int random_map;

	int rows = 0;
	int columns = 0;
	bool holmes = true;
	

	if (p_gp->mCurrentMap > -1)
	{
		random_map = p_gp->mCurrentMap;
	}
	else
	{
		random_map = rand() % 3;
		p_gp->mCurrentMap = random_map;


	}

	// 3 preset mapsizes
	switch (random_map)
	{
	case 0:
		rows = 15;
		columns = 15;
		break;
	case 1:
		rows = 18;
		columns = 18;
		break;
	case 2:
		rows = 24;
		columns = 24;
		break;
	default:
		// For testing the map can be set to close to maximum
		rows = MAX_ARENA_ROWS - 3;
		columns = MAX_ARENA_COLUMNS - 3;
		break;
	}

	GridEcsFunctions::CreateGrid(rECS, rows, columns, 1.0f, holmes);
	InitBiomes(rECS, rows, columns);
}

void CreateCollisionForTiles(ecs::EntityComponentSystem& rECS)
{
	// Collision component.
	ecs::components::ObjectCollisionComponent collision;
	collision.mBvType = COLLISION_CYLINDER;
	collision.mObjType = GAME_OBJECT_TYPE_TILE;

	ecs::TypeFilter filter;
	filter.addRequirement(ecs::components::TileComponent::typeID);
	
	// Grab all tiles.
	ecs::EntityIterator it = rECS.getEntititesByFilter(filter);
	for (ecs::FilteredEntity entity : it.entities)
	{
		// Don't if water.
		if (rECS.getComponentFromEntity<ecs::components::TileComponent>(entity.entity->getID())->tileType != WATER)
		{
			
			rECS.createComponent<ecs::components::ObjectCollisionComponent>(entity.entity->getID(), collision);
		}
	}
}