#include "UtilityFunctions.h"

namespace UtilityFunctions
{
	DirectX::XMMATRIX GetWorldMatrix(ecs::components::TransformComponent tc)
	{
		DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(tc.scale.x, tc.scale.y, tc.scale.z);
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(tc.position.x, tc.position.y, tc.position.z);

		world = scale * translation;
		return world;
	}
}