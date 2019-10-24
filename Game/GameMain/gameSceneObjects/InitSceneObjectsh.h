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
		p_scene_obj.mObject = (SCENEOBJECT)(i%12);
		// Assigns the property of the sceneobject
		p_scene_obj.mObjectType = (OBJECTTYPE)(i%3);
		// assign the filepath to
		p_scene_obj.AssignModel();


		rECS.createEntity(p_scene_obj, transf_comp, color_comp);
	}


	ecs::ComponentIterator itt;
	itt = rECS.getAllComponentsOfType(ecs::components::TileComponent::typeID);
	ecs::ComponentIterator itt2;
	itt2 = rECS.getAllComponentsOfType(ecs::components::SceneObjectComponent::typeID);


	GridProp* p_gp = GridProp::GetInstance();

	int mapsize = p_gp->GetSize().x * p_gp->GetSize().y;

	// Randoms where the sceneobjects should be placed on the map
	int sceneobject_locations[12];
	int kk = 0;
	sceneobject_locations[kk++] = (rand() % mapsize);
	sceneobject_locations[kk++] = (rand() % mapsize);
	sceneobject_locations[kk++] = (rand() % mapsize);
	sceneobject_locations[kk++] = (rand() % mapsize);

	sceneobject_locations[kk++] = (rand() % mapsize);
	sceneobject_locations[kk++] = (rand() % mapsize);
	sceneobject_locations[kk++] = (rand() % mapsize);
	sceneobject_locations[kk++] = (rand() % mapsize);

	sceneobject_locations[kk++] = (rand() % mapsize);
	sceneobject_locations[kk++] = (rand() % mapsize);
	sceneobject_locations[kk++] = (rand() % mapsize);
	sceneobject_locations[kk++] = (rand() % mapsize);

	for (size_t i = 0; i < mapsize; i++)
	{
		// Pass over all tiles
		ecs::components::TileComponent* tile_comp = (ecs::components::TileComponent*)itt.next();
		
		// Look for what tiles the sceneobjects should be placed on
		if (
			sceneobject_locations[0] == i ||
			sceneobject_locations[1] == i ||
			sceneobject_locations[2] == i ||
			sceneobject_locations[3] == i ||
			sceneobject_locations[4] == i ||
			sceneobject_locations[5] == i ||
			sceneobject_locations[6] == i ||
			sceneobject_locations[7] == i ||
			sceneobject_locations[8] == i ||
			sceneobject_locations[9] == i ||
			sceneobject_locations[10] == i ||
			sceneobject_locations[11] == i
			)
		{
			ecs::components::TransformComponent* p_tile_transf_comp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(tile_comp->getEntityID());
			ecs::components::ColorComponent* p_tile_color_comp = rECS.getComponentFromEntity<ecs::components::ColorComponent>(tile_comp->getEntityID());

			ecs::components::SceneObjectComponent* p_scene_comp = (ecs::components::SceneObjectComponent*)itt2.next();
			ecs::components::TransformComponent* p_scene_tranf_comp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(p_scene_comp->getEntityID());
			ecs::components::ColorComponent* p_scene_color_comp = rECS.getComponentFromEntity<ecs::components::ColorComponent>(p_scene_comp->getEntityID());

			//Same transfer component
			p_scene_tranf_comp->position.x = p_tile_transf_comp->position.x;
			p_scene_tranf_comp->position.y = p_tile_transf_comp->position.y;
			p_scene_tranf_comp->position.z = p_tile_transf_comp->position.z;

			// A little darker color
			p_scene_color_comp->red = p_tile_color_comp->red *0.8f;
			p_scene_color_comp->green = p_tile_color_comp->green *0.8f;
			p_scene_color_comp->blue = p_tile_color_comp->blue *0.8f;

			// Changes the modle depening on the biome
			p_scene_comp->ChangeModelByBiome(tile_comp->biome);


			p_gp->mSceneObjects[(i % 12)][(i / 12) - 1] = p_scene_comp->mObjectType;

			if (p_scene_comp->mObjectType == OBJECTTYPE::IMPASSABLE)
			{
				tile_comp->impassable = true;
			}
			else if (p_scene_comp->mObjectType == OBJECTTYPE::INTERESTING)
			{
				// Place it in the grids list of interesting sceneobjects
			}

		}
	}

	

}
