#pragma once
#include "ecs.h"

#include "SceneObjectComponents.h"	

#include "..//gameUtility/UtilityComponents.h"

void InitSceneObjects(ecs::EntityComponentSystem& rECS)
{
	const unsigned int OBJECTCOUNT = 12;
	
	
	ecs::components::SceneObjectComponent scene_obj;
	ecs::components::TransformComponent transf_comp;
	for (size_t i = 0; i < OBJECTCOUNT; i++)
	{
		// Simplefier to handle what object it is
		scene_obj.mObject = (SCENEOBJECT)(i%12);
		// Assigns the property of the sceneobject
		scene_obj.mObjectType = (OBJECTTYPE)(i%3);
		// assign the filepath to
		scene_obj.AssignModel();


		rECS.createEntity(scene_obj, transf_comp);
	}


	ecs::ComponentIterator itt;
	itt = rECS.getAllComponentsOfType(ecs::components::TileComponent::typeID);
	ecs::ComponentIterator itt2;
	itt2 = rECS.getAllComponentsOfType(ecs::components::SceneObjectComponent::typeID);

	for (size_t i = 0; i < 144; i++)
	{
		ecs::components::TileComponent* tile_comp = (ecs::components::TileComponent*)itt.next();

		tile_comp->biome = (BIOME)(i / 36);

		ecs::components::TransformComponent* tile_transf_comp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(tile_comp->getEntityID());


		// each 12th tile gets a sceneobject
		if (i % (144 / 12) == 0)
		{
			ecs::components::SceneObjectComponent* scene_comp = (ecs::components::SceneObjectComponent*)itt2.next();
			ecs::components::TransformComponent* scene_tranf_comp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(scene_comp->getEntityID());

			scene_tranf_comp->position.x = tile_transf_comp->position.x;
			scene_tranf_comp->position.y = tile_transf_comp->position.y + 2.0f;
			scene_tranf_comp->position.z = tile_transf_comp->position.z;


			

		}


	}

	

}
