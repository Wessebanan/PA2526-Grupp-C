#pragma once
#include "GridProp.h"
#include "ecs.h"

#include "SceneObjectComponents.h"	

#include "..//gameUtility/UtilityComponents.h"

void InitSceneObjects(ecs::EntityComponentSystem& rECS)
{
	const unsigned int OBJECTCOUNT = 12;
	
	ecs::components::SceneObjectComponent p_scene_obj;
	ecs::components::TransformComponent transf_comp;
	ecs::components::ColorComponent	color_comp;
	for (size_t i = 0; i < OBJECTCOUNT; i++)
	{
		// Simplefier to handle what object it is
		//p_scene_obj.mObject = (SCENE_OBJECT)(i%12);
		// Assigns the property of the sceneobject
		p_scene_obj.mObjectType = (OBJECT_TYPE)(i%3);
		// assign the filepath to
		p_scene_obj.AssignModel();


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

		// Check occupied flag or if the tile is water
		if (occupied || map_tile_iterator.entities[rand_index].getComponent<TileComponent>()->tileType == WATER)
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
		p_scene_comp = r_obj.getComponent<SceneObjectComponent>();
		p_obj_transform_comp = r_obj.getComponent<TransformComponent>();
		p_obj_color_cmp = r_obj.getComponent<ColorComponent>();

		p_tile_comp = map_tile_iterator.entities[random_tile_indicess[index]].getComponent<TileComponent>();
		p_tile_transform_comp = map_tile_iterator.entities[random_tile_indicess[index]].getComponent<TransformComponent>();

		p_scene_comp->ChangeModelByBiome(p_tile_comp->biome);
		p_obj_transform_comp->position = p_tile_transform_comp->position;

		p_obj_color_cmp->red = rand() % 130 + 50;
		p_obj_color_cmp->green = rand() % 130 + 50;
		p_obj_color_cmp->blue = rand() % 130 + 50;

		index++;
	}

	delete random_tile_indicess; // Unhand me, prieeest























	//ecs::ComponentIterator itt;
	//itt = rECS.getAllComponentsOfType(ecs::components::TileComponent::typeID);
	//ecs::ComponentIterator itt2;
	//itt2 = rECS.getAllComponentsOfType(ecs::components::SceneObjectComponent::typeID);

	//GridProp* p_gp = GridProp::GetInstance();

	//int mapsize = p_gp->GetSize().x * p_gp->GetSize().y;

	// Randoms where the sceneobjects should be placed on the map
	//int sceneobject_locations[12];
	//int kk = 0;
	//sceneobject_locations[kk++] = (rand() % mapsize);
	//sceneobject_locations[kk++] = (rand() % mapsize);
	//sceneobject_locations[kk++] = (rand() % mapsize);
	//sceneobject_locations[kk++] = (rand() % mapsize);

	//sceneobject_locations[kk++] = (rand() % mapsize);
	//sceneobject_locations[kk++] = (rand() % mapsize);
	//sceneobject_locations[kk++] = (rand() % mapsize);
	//sceneobject_locations[kk++] = (rand() % mapsize);

	//sceneobject_locations[kk++] = (rand() % mapsize);
	//sceneobject_locations[kk++] = (rand() % mapsize);
	//sceneobject_locations[kk++] = (rand() % mapsize);
	//sceneobject_locations[kk++] = (rand() % mapsize);






	//for (size_t i = 0; i < mapsize; i++)
	//{
	//	// Pass over all tiles
	//	ecs::components::TileComponent* tile_comp = (ecs::components::TileComponent*)itt.next();
	//	
	//	// Look for what tiles the sceneobjects should be placed on
	//	if (
	//		sceneobject_locations[0] == i ||
	//		sceneobject_locations[1] == i ||
	//		sceneobject_locations[2] == i ||
	//		sceneobject_locations[3] == i ||
	//		sceneobject_locations[4] == i ||
	//		sceneobject_locations[5] == i ||
	//		sceneobject_locations[6] == i ||
	//		sceneobject_locations[7] == i ||
	//		sceneobject_locations[8] == i ||
	//		sceneobject_locations[9] == i ||
	//		sceneobject_locations[10] == i ||
	//		sceneobject_locations[11] == i
	//		)
	//	{
	//		ecs::components::TransformComponent* p_tile_transf_comp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(tile_comp->getEntityID());
	//		ecs::components::ColorComponent* p_tile_color_comp = rECS.getComponentFromEntity<ecs::components::ColorComponent>(tile_comp->getEntityID());

	//		ecs::components::SceneObjectComponent* p_scene_comp = (ecs::components::SceneObjectComponent*)itt2.next();
	//		ecs::components::TransformComponent* p_scene_tranf_comp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(p_scene_comp->getEntityID());
	//		ecs::components::ColorComponent* p_scene_color_comp = rECS.getComponentFromEntity<ecs::components::ColorComponent>(p_scene_comp->getEntityID());

	//		//Same transfer component
	//		p_scene_tranf_comp->position.x = p_tile_transf_comp->position.x;
	//		p_scene_tranf_comp->position.y = p_tile_transf_comp->position.y;
	//		p_scene_tranf_comp->position.z = p_tile_transf_comp->position.z;

	//		// A little darker color
	//		p_scene_color_comp->red = p_tile_color_comp->red *0.5f;
	//		p_scene_color_comp->green = p_tile_color_comp->green *0.5f;
	//		p_scene_color_comp->blue = p_tile_color_comp->blue *0.5f;

	//		// Changes the modle depening on the biome
	//		p_scene_comp->ChangeModelByBiome(tile_comp->biome);


	//		p_gp->mSceneObjects[(i % 12)][(i / 12) - 1] = p_scene_comp->mObjectType;	// HARD CODED 12?

	//		if (p_scene_comp->mObjectType == OBJECT_TYPE::IMPASSABLE)
	//		{
	//			tile_comp->impassable = true;
	//		}
	//		else if (p_scene_comp->mObjectType == OBJECT_TYPE::INTERESTING)
	//		{
	//			// Place it in the grids list of interesting sceneobjects
	//		}

	//	}
	//}

	

}
