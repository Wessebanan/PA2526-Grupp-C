#pragma once

#include "ecsComponentIncludes.h"
#include <directXMath.h>

using namespace DirectX;

namespace ecs
{
	namespace components
	{
		struct CameraComponent : public ECSComponent<CameraComponent>
		{
			XMVECTOR target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			XMVECTOR right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
			
			XMMATRIX projectionMatrix;
			XMMATRIX viewMatrix;
		};
	}
}