#include "InitWorldScenery.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameWorld/WorldComponents.h"
#include "../Physics/PhysicsComponents.h"
#include "../gameWorld/WorldScenerySystems.h"
#include <DirectXMath.h>
#include "../gameAI/AIComponents.h"

using namespace ecs;
using namespace DirectX;

void InitWorldScenery(ecs::EntityComponentSystem& rECS)
{
	TypeFilter map_filter;
	map_filter.addRequirement(components::TileComponent::typeID);
	map_filter.addRequirement(components::TransformComponent::typeID);
	EntityIterator tiles = rECS.getEntititesByFilter(map_filter);

	int valid_tiles = 0;
	XMVECTOR center = { 0.f, 0.f, 0.f, 0.f };
	for (FilteredEntity tile : tiles.entities)
	{
		if (tile.getComponent<components::TileComponent>()->tileType != WATER)
		{
			center += XMLoadFloat3(&tile.getComponent<components::TransformComponent>()->position);
			valid_tiles++;
		}
	}

	XMFLOAT3 map_center;

	center /= (float)valid_tiles;
	XMStoreFloat3(&map_center, center);
	map_center.y = -1.2f;

	ecs::components::TransformComponent trans_comp;
	ecs::components::ColorComponent color_comp;
	ecs::components::WorldSceneryComponent shark_comp;

	// Init values for the shark
	shark_comp.sceneryType = GAME_OBJECT_TYPE_WORLD_SCENE_SHARK;
	color_comp.red	 = 75;
	color_comp.green = 75;
	color_comp.blue  = 75;
	trans_comp.position = map_center;
	trans_comp.position.x += 22.5f;
	trans_comp.scale.x *= 5;
	trans_comp.scale.y *= 5;
	trans_comp.scale.z *= 5;
	

	rECS.createEntity(trans_comp, color_comp, shark_comp);
	trans_comp.position = map_center;
	trans_comp.position.x -= 22.5f;
	rECS.createEntity(trans_comp, color_comp, shark_comp);
	trans_comp.position = map_center;
	trans_comp.position.z += 22.5f;
	rECS.createEntity(trans_comp, color_comp, shark_comp);
	trans_comp.position = map_center;
	trans_comp.position.z -= 22.5f;
	rECS.createEntity(trans_comp, color_comp, shark_comp);
	rECS.createSystem<ecs::systems::WorldSceneryUpdateSystem>(9)->Initialize();
}
