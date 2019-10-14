#include "UtilityEcsFunctions.h"

namespace UtilityEcsFunctions
{
	DirectX::XMMATRIX GetWorldMatrix(ecs::components::TransformComponent tc)
	{
		DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(tc.scale.x, tc.scale.y, tc.scale.z);
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(tc.position.x, tc.position.y, tc.position.z);
		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(tc.rotation.x, tc.rotation.y, tc.rotation.z);

		world = scale * rotation * translation; 
		return world;
	}
}