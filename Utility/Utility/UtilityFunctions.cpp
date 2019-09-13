#include "UtilityFunctions.h"

namespace UtilityFunctions
{
	XMMATRIX GetWorldMatrix(ecs::components::TransformComponent tc)
	{
		XMMATRIX world = XMMatrixIdentity();
		XMMATRIX scale = XMMatrixScaling(tc.scaleX, tc.scaleY, tc.scaleZ);
		XMMATRIX translation = XMMatrixTranslation(tc.positionX, tc.positionY, tc.positionZ);

		world = scale * translation;
		return world;
	}
}