#pragma once
#include "GridProp.h"
#include "ecs.h"

#include "SceneObjectComponents.h"	

#include "..//gameUtility/UtilityComponents.h"

#include "../gameWorld/WorldComponents.h"

void InitSceneObjects(ecs::EntityComponentSystem& rECS)
{
	const unsigned int OBJECTCOUNT = 12;
	
	ecs::components::SceneObjectComponent p_scene_obj;
	ecs::components::TransformComponent transf_comp;
	ecs::components::ColorComponent	color_comp;
	
	for (size_t i = 0; i < OBJECTCOUNT; i++)
	{
		p_scene_obj.mObjectType = (OBJECT_TYPE)(i%3);
		// assign the filepath to


		rECS.createEntity(p_scene_obj, transf_comp, color_comp);
	}


	

	TypeFilter scene_obj_filter;
	scene_obj_filter.addRequirement(components::SceneObjectComponent::typeID);
	scene_obj_filter.addRequirement(components::TransformComponent::typeID);
	scene_obj_filter.addRequirement(components::ColorComponent::typeID);

	TypeFilter map_tile_filter;
	map_tile_filter.addRequirement(components::TileComponent::typeID);
	map_tile_filter.addRequirement(components::TransformComponent::typeID);

	EntityIterator scene_obj_iterator = rECS.getEntititesByFilter(scene_obj_filter);
	EntityIterator map_tile_iterator = rECS.getEntititesByFilter(map_tile_filter);


	const int MAP_TILE_COUNT = (int)map_tile_iterator.entities.size();
	int* random_tile_indicess = new int[scene_obj_iterator.entities.size()];

	int index = 0;
	while (index < (int)scene_obj_iterator.entities.size())
	{
		bool occupied = false;
		int rand_index = rand() % MAP_TILE_COUNT;

		// Check if index is already occupied
		for (int i = index - 1; i >= 0 && !occupied; i--)
		{
			if (random_tile_indicess[i] == rand_index)
			{
				occupied = true;
				break;
			}
		}

		// Check occupied flag or if the tile is water or if the tile is part of an islet.
		if (occupied ||
			map_tile_iterator.entities[rand_index].getComponent<TileComponent>()->tileType == WATER || 
			map_tile_iterator.entities[rand_index].entity->hasComponentOfType<IsletComponent>())
		{
			continue;
		}

		random_tile_indicess[index++] = rand_index;
	}

	index = 0;
	TileComponent* p_tile_comp;
	SceneObjectComponent* p_scene_comp;
	ColorComponent* p_obj_color_cmp;

	TransformComponent* p_obj_transform_comp;
	TransformComponent* p_tile_transform_comp;

	for (FilteredEntity& r_obj : scene_obj_iterator.entities)
	{
		float r = rand();

		p_scene_comp = r_obj.getComponent<SceneObjectComponent>();
		p_obj_transform_comp = r_obj.getComponent<TransformComponent>();
		p_obj_color_cmp = r_obj.getComponent<ColorComponent>();

		p_tile_comp = map_tile_iterator.entities[random_tile_indicess[index]].getComponent<TileComponent>();
		p_tile_comp->impassable = true;
		p_tile_transform_comp = map_tile_iterator.entities[random_tile_indicess[index]].getComponent<TransformComponent>();

		p_scene_comp->ChangeModelByBiome(p_tile_comp->biome);
		p_obj_transform_comp->position = p_tile_transform_comp->position;
		p_obj_transform_comp->rotation.y = (r / (float)RAND_MAX) * 3.14f * 2.0f;

		p_obj_color_cmp->red = rand() % 130 + 50;
		p_obj_color_cmp->green = rand() % 130 + 50;
		p_obj_color_cmp->blue = rand() % 130 + 50;

		index++;
	}

	delete random_tile_indicess;
}
