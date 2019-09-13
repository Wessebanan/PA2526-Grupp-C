#pragma once

#include "ecsComponentIncludes.h"

namespace ecs
{
	namespace components
	{
		struct TransformComponent : public ECSComponent<TransformComponent>
		{
			float positionX = 0.0f, positionY = 0.0f, positionZ = 0.0f;
			float rotationX = 0.0f, rotationY = 0.0f, rotationZ = 0.0f;
			float scaleX = 0.0f, scaleY = 0.0f, scaleZ = 0.0f;
		};
	}
}