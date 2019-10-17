#pragma once

#include "rendering/RenderManager.h"
#include "ecs.h"
#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameSceneObjects/SceneObjectGlobals.h"

void PlaceMesh(ecs::EntityComponentSystem& rECS, rendering::RenderManager* mng)
{
	using namespace rendering;
	using namespace DirectX;
	struct float4
	{
		float x, y, z;
		uint32_t color;
	};
	float4* pTilePosition = (float4*)mng->GetTechniqueModelBuffer(RENDER_DEFAULT);
	XMFLOAT4X4* p_unit_pos = (XMFLOAT4X4*)mng->GetTechniqueModelBuffer(RENDER_TRANSFORMATION);

	UINT index = 0;
	ecs::ComponentIterator itt;

	//InitGravityOnUnits(ecs, dude);




	itt = rECS.getAllComponentsOfType(ecs::components::TileComponent::typeID);
	ecs::components::TileComponent* tileComp;
	while (tileComp = (ecs::components::TileComponent*)itt.next())
	{
		ecs::components::TransformComponent* trComp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(tileComp->getEntityID());
		ecs::components::ColorComponent* color_comp = rECS.getComponentFromEntity<ecs::components::ColorComponent>(tileComp->getEntityID());

		ecs::components::MeshComponent* mesh_comp = rECS.getComponentFromEntity<ecs::components::MeshComponent>(tileComp->getEntityID());

		pTilePosition[index].x = trComp->position.x;
		pTilePosition[index].y = trComp->position.y;
		pTilePosition[index].z = trComp->position.z;

		int random = rand() % 101;
		int color_offset = -50 + random;
		switch (tileComp->tileType)
		{
		case TileTypes::GAME_FIELD:
			pTilePosition[index].color = PACK(color_comp->red, color_comp->green, color_comp->blue, 0);
			break;
		case TileTypes::WATER:
			pTilePosition[index].color = PACK(0, 0, 200 + color_offset, 0);
			break;
		case TileTypes::UNDEFINED:
			pTilePosition[index].color = PACK(0, 0, 0, 255);
			break;
		default:
			pTilePosition[index].color = PACK(255, 255, 255, 255);
			break;
		}

		index++;
	}

	itt = rECS.getAllComponentsOfType(ecs::components::SceneObjectComponent::typeID);
	ecs::components::SceneObjectComponent* scene_comp;
	while (scene_comp = (ecs::components::SceneObjectComponent*)itt.next())
	{
		ecs::components::TransformComponent* trComp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(scene_comp->getEntityID());
		ecs::components::ColorComponent* color_comp = rECS.getComponentFromEntity<ecs::components::ColorComponent>(scene_comp->getEntityID());

		pTilePosition[index].x = trComp->position.x;
		pTilePosition[index].y = trComp->position.y;
		pTilePosition[index].z = trComp->position.z;

		pTilePosition[index].color = PACK(color_comp->red, color_comp->green, color_comp->blue, 0);
		

		index++;
	}

	int armyIndex = 0;
	itt = rECS.getAllComponentsOfType(ecs::components::ArmyComponent::typeID);
	ecs::components::ArmyComponent* armComp;
	while (armComp = (ecs::components::ArmyComponent*)itt.next())
	{
		// over alla units in the army
		for (size_t i = 0; i < 3; i++)
		{
			ecs::components::TransformComponent* trComp = rECS.getComponentFromEntity<ecs::components::TransformComponent>(armComp->unitIDs[i]);

			XMMATRIX world = XMMatrixIdentity();
			world *= XMMatrixScaling(trComp->scale.x, trComp->scale.y, trComp->scale.z);
			world *= XMMatrixRotationRollPitchYaw(trComp->rotation.x, trComp->rotation.y, trComp->rotation.z);
			world *= XMMatrixTranslation(trComp->position.x, trComp->position.y, trComp->position.z);

			XMStoreFloat4x4(&p_unit_pos[armyIndex], world);

			armyIndex++;
		}
	}

}