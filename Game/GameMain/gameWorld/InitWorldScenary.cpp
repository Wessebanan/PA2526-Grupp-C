#include "InitWorldScenary.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameWorld/WorldComponents.h"
#include "../Physics/PhysicsComponents.h"
#include "../gameWorld/WorldScenerySystems.h"
#include <DirectXMath.h>

using namespace DirectX;
void InitWorldScenary(ecs::EntityComponentSystem& rECS)
{
	ecs::components::TransformComponent trans_comp;
	ecs::components::ColorComponent color_comp;
	ecs::components::WorldSceneryComponent shark_comp;
	ecs::components::DynamicMovementComponent shark_move_comp;

	shark_comp.sceneryType = GAME_OBJECT_TYPE_WORLD_SCENE_SHARK;
	color_comp.red	 = 75;
	color_comp.green = 75;
	color_comp.blue  = 75;
	trans_comp.position = XMFLOAT3(0.f, -1.f, 0.f);
	trans_comp.scale.x *= 5;
	trans_comp.scale.y *= 5;
	trans_comp.scale.z *= 5;
	shark_move_comp.mGravity = 0.f;
	shark_move_comp.mOnGround = true;
	shark_move_comp.mMaxVelocity = 0.15f;

	rECS.createEntity(trans_comp, color_comp, shark_comp, shark_move_comp);
	rECS.createSystem<ecs::systems::WorldSceneryUpdateSystem>(9);
}
